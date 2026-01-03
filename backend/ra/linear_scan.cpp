#include <backend/ra/linear_scan.h>
#include <backend/mir/m_function.h>
#include <backend/mir/m_instruction.h>
#include <backend/mir/m_block.h>
#include <backend/mir/m_defs.h>
#include <backend/target/target_reg_info.h>
#include <backend/target/target_instr_adapter.h>
#include <backend/common/cfg.h>
#include <backend/common/cfg_builder.h>
#include <utils/dynamic_bitset.h>
#include <debug.h>

#include <map>
#include <set>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <optional>

namespace BE::RA
{
    /*
     * 线性扫描寄存器分配（Linear Scan）教学版说明
     *
     * 目标：将每个虚拟寄存器（vreg）的活跃区间映射到目标机的物理寄存器或栈槽（溢出）。
     *
     * 核心步骤（整数/浮点分开执行，流程相同）：
     * 1) 指令线性化与编号：为函数内所有指令分配全局顺序号，记录每个基本块的 [start, end) 区间，
     *    同时收集调用点（callPoints），用于偏好分配被调用者保存寄存器（callee-saved）。
     * 2) 构建 USE/DEF：枚举每条指令的使用与定义寄存器，聚合到基本块级的 USE/DEF 集合。
     * 3) 活跃性分析：在 CFG 上迭代 IN/OUT，满足 IN = USE ∪ (OUT − DEF) 直至收敛。
     * 4) 活跃区间构建：按基本块从后向前，根据 IN/OUT 与指令次序，累积每个 vreg 的若干 [start, end) 段并合并。
     * 5) 标记跨调用：若区间与任意调用点重叠（交叉），标记 crossesCall=true，以便后续优先使用被调用者保存寄存器。
     * 6) 线性扫描分配：将区间按起点排序，维护活动集合 active；到达新区间时先移除已过期区间，然后
     *    尝试选择空闲物理寄存器；若无空闲则选择一个区间溢出（常见启发：溢出“结束点更远”的区间）。
     * 7) 重写 MIR：对未分配物理寄存器的 use/def，在指令前/后插入 reload/spill，并用临时物理寄存器替换操作数。
     *
     * 提示：
     * - 通过 TargetInstrAdapter 提供的接口完成目标无关的指令读写。
     * - TargetRegInfo 提供了可分配寄存器集合、被调用者保存寄存器、保留寄存器等信息。
     */
    namespace
    {
        // 区间段结构体
        struct Segment
        {
            int start;
            int end;
            Segment(int s = 0, int e = 0) : start(s), end(e) {}
        };
        // 活跃区间结构体
        struct Interval
        {
            BE::Register         vreg;
            std::vector<Segment> segs;
            bool                 crossesCall = false; // 是否跨越函数调用

            void addSegment(int s, int e)
            {
                if (s >= e) return;
                segs.emplace_back(s, e);
            }
            // 合并重叠区间
            void merge()
            {
                if (segs.empty()) return;
                std::sort(segs.begin(), segs.end(), [](const Segment& a, const Segment& b) {
                    if (a.start != b.start) return a.start < b.start;
                    return a.end < b.end;
                });
                std::vector<Segment> merged;
                merged.push_back(segs[0]);
                for (size_t i = 1; i < segs.size(); ++i)
                {
                    auto& last = merged.back();
                    if (segs[i].start <= last.end)
                        last.end = std::max(last.end, segs[i].end);
                    else
                        merged.push_back(segs[i]);
                }
                segs.swap(merged);
            }
        };

        // 区间排序规则（按起点排序，起点相同按终点排序）
        struct IntervalOrder
        {
            bool operator()(const Interval* a, const Interval* b) const
            {
                int astart = a->segs.empty() ? 0 : a->segs.front().start;
                int bstart = b->segs.empty() ? 0 : b->segs.front().start;
                if (astart != bstart) return astart < bstart;
                int aend = a->segs.empty() ? 0 : a->segs.back().end;
                int bend = b->segs.empty() ? 0 : b->segs.back().end;
                return aend < bend;
            }
        };
    }  // namespace

    // 构建可分配的整数寄存器集合
    static std::vector<int> buildAllocatableInt(const BE::Targeting::TargetRegInfo& ri)
    {
        std::vector<int> res;
        auto reserved = ri.reservedRegs();
        auto ints     = ri.intRegs();
        for (int r : ints)
            if (std::find(reserved.begin(), reserved.end(), r) == reserved.end()) res.push_back(r);
        return res;
    }
    // 构建可分配的浮点寄存器集合
    static std::vector<int> buildAllocatableFloat(const BE::Targeting::TargetRegInfo& ri)
    {
        std::vector<int> res;
        auto reserved = ri.reservedRegs();
        auto floats   = ri.floatRegs();
        for (int r : floats)
            if (std::find(reserved.begin(), reserved.end(), r) == reserved.end()) res.push_back(r);
        return res;
    }

    void LinearScanRA::allocateFunction(BE::Function& func, const BE::Targeting::TargetRegInfo& regInfo)
    {
        ASSERT(BE::Targeting::g_adapter && "TargetInstrAdapter is not set");

        // 指令线性化与编号，收集调用点
        std::map<BE::Block*, std::pair<int, int>>                                   blockRange;
        std::vector<std::pair<BE::Block*, std::deque<BE::MInstruction*>::iterator>> id2iter;
        std::set<int>                                                               callPoints;
        int                                                                         ins_id = 0;
        for (auto& [bid, block] : func.blocks)
        {
            int start = ins_id;
            for (auto it = block->insts.begin(); it != block->insts.end(); ++it, ++ins_id)
            {
                id2iter.emplace_back(block, it);
                if (BE::Targeting::g_adapter->isCall(*it)) callPoints.insert(ins_id);
            }
            blockRange[block] = {start, ins_id};
        }

        // 构建 USE/DEF 集合
        std::map<BE::Block*, std::set<BE::Register>> USE, DEF;
        for (auto& [bid, block] : func.blocks)
        {
            std::set<BE::Register> use, def;
            for (auto it = block->insts.begin(); it != block->insts.end(); ++it)
            {
                std::vector<BE::Register> uses, defs;
                BE::Targeting::g_adapter->enumUses(*it, uses);
                BE::Targeting::g_adapter->enumDefs(*it, defs);
                for (auto& d : defs)
                    if (!def.count(d)) def.insert(d);
                for (auto& u : uses)
                    if (!def.count(u)) use.insert(u);
            }
            USE[block] = std::move(use);
            DEF[block] = std::move(def);
        }

        // ============================================================================
        // 构建 CFG 后继关系
        // ============================================================================
        // 作用：搭建活跃性数据流的图结构。
        // 如何做：可直接用 MIR::CFGBuilder 生成 CFG，再转换为 succs 映射。
        BE::MIR::CFG*                                 cfg = nullptr;
        std::map<BE::Block*, std::vector<BE::Block*>> succs;
        {
            BE::MIR::CFGBuilder builder(BE::Targeting::g_adapter);
            cfg = builder.buildCFGForFunction(&func);
            if (cfg)
            {
                for (auto& [bid, block] : func.blocks)
                {
                    std::vector<BE::Block*> outs;
                    for (auto succId : cfg->graph_id[bid])
                    {
                        if (func.blocks.count(succId)) outs.push_back(func.blocks[succId]);
                    }
                    succs[block] = std::move(outs);
                }
            }
        }

        // ============================================================================
        // 活跃性分析（IN/OUT）
        // ============================================================================
        // IN[b] = USE[b] ∪ (OUT[b] − DEF[b])，OUT[b] = ⋃ IN[s]，其中 s ∈ succs[b]
        // 迭代执行上述操作直到不变为止
        std::map<BE::Block*, std::set<BE::Register>> IN, OUT;
        bool                                         changed = true;
        while (changed)
        {
            changed = false;
            for (auto& [bid, block] : func.blocks)
            {
                std::set<BE::Register> newOUT;
                for (auto* s : succs[block])
                {
                    auto it = IN.find(s);
                    if (it != IN.end()) newOUT.insert(it->second.begin(), it->second.end());
                }
                std::set<BE::Register> newIN = USE[block];

                for (auto& r : newOUT)
                    if (!DEF[block].count(r)) newIN.insert(r);

                if (!(newOUT != OUT[block] || newIN != IN[block])) continue;

                OUT[block] = std::move(newOUT);
                IN[block]  = std::move(newIN);
                changed    = true;
            }
        }

        delete cfg;

        // ============================================================================
        // 构建活跃区间（Intervals）
        // ============================================================================
        // 作用：得到每个 vreg 的若干 [start,end) 段并合并（interval.merge()）。
        // 如何做：对每个基本块，反向遍历其指令序列，根据 IN/OUT/uses/defs 更新段的开始/结束。
        std::map<BE::Register, Interval> intervals;

        // 辅助函数
        auto ensureInterval = [&](const BE::Register& v) -> Interval& {
            auto it = intervals.find(v);
            if (it == intervals.end())
            {
                Interval iv;
                iv.vreg = v;
                auto [nit, _] = intervals.emplace(v, std::move(iv));
                return nit->second;
            }
            return it->second;
        };

        // block live-out start map
        for (auto& [bid, block] : func.blocks)
        {
            (void)bid;
            int start = blockRange[block].first;
            int end   = blockRange[block].second;

            std::map<BE::Register, int> liveBeg;
            auto outSet = OUT[block];
            for (auto& r : outSet)
                if (r.isVreg) liveBeg[r] = end;

            for (int id = end - 1; id >= start; --id)
            {
                auto [b, it] = id2iter[id];
                (void)b;
                std::vector<BE::Register> uses, defs;
                BE::Targeting::g_adapter->enumUses(*it, uses);
                BE::Targeting::g_adapter->enumDefs(*it, defs);

                // defs: close live ranges
                for (auto& d : defs)
                {
                    if (!d.isVreg) continue;
                    int segEnd = liveBeg.count(d) ? liveBeg[d] : (id + 1);
                    ensureInterval(d).addSegment(id, segEnd);
                    liveBeg.erase(d);
                }

                // uses: extend live
                for (auto& u : uses)
                {
                    if (!u.isVreg) continue;
                    auto itb = liveBeg.find(u);
                    if (itb == liveBeg.end()) liveBeg[u] = id + 1;
                    else itb->second = std::max(itb->second, id + 1);
                }
            }

            // remaining live at block entry
            for (auto& [r, endPos] : liveBeg)
                ensureInterval(r).addSegment(start, endPos);
        }

        // 合并并标记跨调用
        for (auto& [r, iv] : intervals)
        {
            iv.merge();
            for (int cp : callPoints)
                for (auto& s : iv.segs)
                    if (s.start <= cp && cp < s.end) iv.crossesCall = true;
        }

        // ============================================================================
        // 线性扫描主循环
        // ============================================================================
        // 作用：按区间起点排序；进入新区间前，先从活动集合 active 移除“已结束”的区间；
        // 然后尝试分配空闲物理寄存器；若无可用，执行溢出策略（如“溢出结束点更远”的区间）。
        auto allIntRegs       = buildAllocatableInt(regInfo);
        auto allFloatRegs     = buildAllocatableFloat(regInfo);
        auto scratchIntRegs   = allIntRegs;
        auto scratchFloatRegs = allFloatRegs;
        std::map<BE::Register, int> assignedPhys;
        std::map<BE::Register, int> spillFI;

        std::vector<Interval*> intIntervals;
        std::vector<Interval*> floatIntervals;
        for (auto& [r, iv] : intervals)
        {
            if (r.dt == BE::F32 || r.dt == BE::F64)
                floatIntervals.push_back(&iv);
            else
                intIntervals.push_back(&iv);
        }

        // 分配函数
        auto assignClass = [&](std::vector<Interval*>& work, std::vector<int>& freeRegs, const std::vector<int>& preferredForCalls) {
            std::sort(work.begin(), work.end(), IntervalOrder());
            std::vector<std::pair<Interval*, int>> active;

            // 选择寄存器的辅助函数
            auto takePreferred = [&](bool wantPreferred) -> std::optional<int> {
                if (freeRegs.empty()) return std::nullopt;
                if (!wantPreferred || preferredForCalls.empty())
                {
                    int preg = freeRegs.back();
                    freeRegs.pop_back();
                    return preg;
                }

                for (auto it = freeRegs.begin(); it != freeRegs.end(); ++it)
                {
                    if (std::find(preferredForCalls.begin(), preferredForCalls.end(), *it) == preferredForCalls.end()) continue;
                    int preg = *it;
                    freeRegs.erase(it);
                    return preg;
                }

                // 如果需要一个被调用者保存的寄存器（跨调用）但没有可用的，
                // 我们必须溢出，因为调用者保存的寄存器会被破坏。
                return std::nullopt;
            };

            // 过期区间处理的辅助函数
            auto expire = [&](int curStart) {
                auto it = active.begin();
                while (it != active.end())
                {
                    int end = it->first->segs.back().end;
                    if (end <= curStart)
                    {
                        freeRegs.push_back(it->second);
                        it = active.erase(it);
                    }
                    else
                        ++it;
                }
            };

            for (auto* iv : work)
            {
                if (iv->segs.empty()) continue;
                int start = iv->segs.front().start;
                expire(start);

                if (!freeRegs.empty())
                {
                    auto pregOpt = takePreferred(iv->crossesCall);
                    if (!pregOpt.has_value())
                    {
                        int fi = func.frameInfo.createSpillSlot(8);
                        spillFI[iv->vreg] = fi;
                        continue;
                    }
                    int preg = *pregOpt;
                    assignedPhys[iv->vreg] = preg;
                    active.push_back({iv, preg});
                }
                else
                {
                    int fi = func.frameInfo.createSpillSlot(8);
                    spillFI[iv->vreg] = fi;
                }
            }
        };

        assignClass(intIntervals, allIntRegs, regInfo.calleeSavedIntRegs());
        assignClass(floatIntervals, allFloatRegs, regInfo.calleeSavedFloatRegs());

        // =========================================================================
        // 重写 MIR（插入 reload/spill，替换 use/def）
        // =========================================================================
        // 注意：block->insts 是 std::deque；在其上 insert 会使迭代器失效。
        // 因此这里采用“重建指令列表”的方式进行改写，避免迭代器失效导致漏改写/未分配 vreg。

        // 选择临时寄存器（scratch reg），避免与已用寄存器冲突
        auto pickScratch = [&](BE::DataType* dt, std::set<int>& used) -> std::optional<BE::Register> {
            const bool isFloat = (dt == BE::F32 || dt == BE::F64);
            // 使用专用的 scratch 寄存器（分配时已保留）：
            // - 整数：x16/x17（I32 时为 w16/w17）
            // - 浮点：d16/d17（F32 时为 s16/s17）
            static const int kScratchIds[2] = {16, 17};
            for (int id : kScratchIds)
            {
                if (used.count(id)) continue;
                used.insert(id);
                BE::DataType* chosenDt = dt;
                if (chosenDt == nullptr) chosenDt = isFloat ? BE::F64 : BE::I64;
                return BE::Register(id, chosenDt, false);
            }
            return std::nullopt;
        };

        for (auto& [bid, block] : func.blocks)
        {
            (void)bid;
            if (!block) continue;

            std::deque<BE::MInstruction*> rewritten;
            rewritten.clear();

            for (auto* inst : block->insts)
            {
                if (!inst)
                {
                    rewritten.push_back(inst);
                    continue;
                }

                std::vector<BE::Register> uses, defs;
                BE::Targeting::g_adapter->enumUses(inst, uses);
                BE::Targeting::g_adapter->enumDefs(inst, defs);

                std::set<int> usedScratchIds;

                // 重写使用：用临时/物理寄存器替换 use，并在指令前插入溢出加载
                for (auto& u : uses)
                {
                    if (!u.isVreg) continue;
                    auto ap = assignedPhys.find(u);
                    if (ap != assignedPhys.end())
                    {
                        BE::Targeting::g_adapter->replaceUse(inst, u, BE::Register(ap->second, u.dt, false));
                        continue;
                    }
                    auto sp = spillFI.find(u);
                    if (sp == spillFI.end()) continue;
                    auto scratch = pickScratch(u.dt, usedScratchIds);
                    if (!scratch.has_value()) continue;
                    rewritten.push_back(new BE::FILoadInst(*scratch, sp->second, ""));
                    BE::Targeting::g_adapter->replaceUse(inst, u, *scratch);
                }

                // 清空已用的临时寄存器，因为后续定义可以复用（输入在输出前已被读取）
                usedScratchIds.clear();

                // 重写定义：用临时/物理寄存器替换 def，并在指令后插入溢出存储
                // （我们先缓存溢出操作，随后追加到指令后）
                std::vector<std::pair<BE::Register, int>> spills;
                spills.reserve(defs.size());
                for (auto& d : defs)
                {
                    if (!d.isVreg) continue;
                    auto ap = assignedPhys.find(d);
                    if (ap != assignedPhys.end())
                    {
                        BE::Targeting::g_adapter->replaceDef(inst, d, BE::Register(ap->second, d.dt, false));
                        continue;
                    }
                    auto sp = spillFI.find(d);
                    if (sp == spillFI.end()) continue;
                    auto scratch = pickScratch(d.dt, usedScratchIds);
                    if (!scratch.has_value()) continue;
                    BE::Targeting::g_adapter->replaceDef(inst, d, *scratch);
                    spills.emplace_back(*scratch, sp->second);
                }

                rewritten.push_back(inst);
                for (auto& [scratchReg, fi] : spills)
                    rewritten.push_back(new BE::FIStoreInst(scratchReg, fi, ""));
            }

            block->insts.swap(rewritten);
        }
    }
}  // namespace BE::RA
