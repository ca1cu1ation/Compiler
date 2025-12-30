#include <backend/targets/aarch64/passes/lowering/phi_elimination.h>
#include <backend/targets/aarch64/aarch64_defs.h>
#include <backend/common/cfg_builder.h>
#include <backend/common/cfg.h>
#include <debug.h>
#include <algorithm>
#include <transfer.h>
#include <map>
#include <set>
#include <vector>

namespace BE::AArch64::Passes::Lowering
{
    using namespace BE;

    namespace
    {
        using Copy = std::pair<Register, Register>;  // dst <- src

        static void rewriteBranchTarget(BE::Block* pred, uint32_t oldTarget, uint32_t newTarget)
        {
            if (!pred) return;
            for (auto* inst : pred->insts)
            {
                if (!inst || inst->kind != BE::InstKind::TARGET) continue;
                auto* ti = static_cast<BE::AArch64::Instr*>(inst);
                auto  ty = BE::AArch64::getOpInfoType(ti->op);
                if (ty != BE::AArch64::OpType::L) continue;
                if (ti->operands.empty()) continue;
                if (auto* lb = dynamic_cast<BE::AArch64::LabelOperand*>(ti->operands[0]))
                {
                    if (static_cast<uint32_t>(lb->targetBlockId) == oldTarget) lb->targetBlockId = static_cast<int>(newTarget);
                }
            }
        }

        static std::vector<BE::MInstruction*> materializeParallelCopies(const std::vector<Copy>& inCopies)
        {
            std::vector<std::pair<Register, Register>> worklist;
            for (auto& [dst, src] : inCopies)
            {
                if (dst == src) continue;
                worklist.push_back({dst, src});
            }

            std::vector<BE::MInstruction*> out;
            if (worklist.empty()) return out;

            std::map<Register, int> useCounts;
            for (auto& [dst, src] : worklist)
            {
                useCounts[src]++;
            }

            while (!worklist.empty())
            {
                bool progressed = false;
                for (auto it = worklist.begin(); it != worklist.end();)
                {
                    Register dst = it->first;
                    Register src = it->second;

                    if (useCounts[dst] == 0)
                    {
                        out.push_back(BE::AArch64::createMove(new BE::RegOperand(dst),
                            new BE::RegOperand(src),
                            LOC_STR));
                        useCounts[src]--;
                        it = worklist.erase(it);
                        progressed = true;
                    }
                    else
                    {
                        ++it;
                    }
                }

                if (progressed) continue;

                // Cycle detected. Break it.
                auto&    copy = worklist.front();
                Register dst  = copy.first;
                Register tmp  = BE::getVReg(dst.dt);

                out.push_back(BE::AArch64::createMove(new BE::RegOperand(tmp),
                    new BE::RegOperand(dst),
                    LOC_STR));

                for (auto& [d, s] : worklist)
                {
                    if (s == dst)
                    {
                        s = tmp;
                        useCounts[dst]--;
                        useCounts[tmp]++;
                    }
                }
            }

            return out;
        }

        static void insertMovesBeforeTerminator(BE::Block* block,
            const std::vector<BE::MInstruction*>& moves,
            const BE::Targeting::TargetInstrAdapter* adapter)
        {
            if (!block || moves.empty()) return;
            if (block->insts.empty())
            {
                for (auto* m : moves) block->insts.push_back(m);
                return;
            }

            auto* last = block->insts.back();
            bool  hasTerm = adapter && (adapter->isUncondBranch(last) || adapter->isCondBranch(last) || adapter->isReturn(last));
            if (!hasTerm)
            {
                for (auto* m : moves) block->insts.push_back(m);
                return;
            }

            std::deque<BE::MInstruction*> rewritten;
            rewritten.insert(rewritten.end(), block->insts.begin(), block->insts.end());
            rewritten.pop_back();
            for (auto* m : moves) rewritten.push_back(m);
            rewritten.push_back(last);
            block->insts.swap(rewritten);
        }
    }  // namespace

    void PhiEliminationPass::runOnModule(BE::Module& module, const BE::Targeting::TargetInstrAdapter* adapter)
    {
        if (module.functions.empty()) return;
        for (auto* func : module.functions) runOnFunction(func, adapter);
    }

    void PhiEliminationPass::runOnFunction(BE::Function* func, const BE::Targeting::TargetInstrAdapter* adapter)
    {
        if (!func || func->blocks.empty()) return;
        ASSERT(adapter && "PhiEliminationPass requires TargetInstrAdapter");

        // Build CFG to query predecessors/successors.
        BE::MIR::CFGBuilder builder(adapter);
        BE::MIR::CFG*       cfg = builder.buildCFGForFunction(func);
        if (!cfg) return;

        uint32_t maxId = 0;
        for (auto& [id, _] : func->blocks) maxId = std::max(maxId, id);

        // Reuse split blocks per edge (pred -> succ).
        std::map<std::pair<uint32_t, uint32_t>, uint32_t> edgeSplit;

        for (auto& [bid, block] : func->blocks)
        {
            if (!block) continue;

            // Collect PHIs in this block.
            std::vector<BE::PhiInst*> phis;
            for (auto* inst : block->insts)
                if (inst && inst->kind == BE::InstKind::PHI) phis.push_back(static_cast<BE::PhiInst*>(inst));

            if (phis.empty()) continue;

            // predId -> copies for that incoming edge
            std::map<uint32_t, std::vector<Copy>> copiesPerPred;
            std::map<uint32_t, std::vector<BE::MInstruction*>> immMovesPerPred;
            std::set<uint32_t> preds;

            for (auto* phi : phis)
            {
                Register dst = phi->resReg;
                for (auto& [predId, op] : phi->incomingVals)
                {
                    preds.insert(predId);
                    if (auto* rop = dynamic_cast<BE::RegOperand*>(op))
                    {
                        copiesPerPred[predId].push_back({dst, rop->reg});
                    }
                    else if (auto* iop = dynamic_cast<BE::AArch64::ImmeOperand*>(op))
                    {
                        int  imm    = iop->value;
                        bool simple = (imm >= 0 && imm <= 65535) || (~imm >= 0 && ~imm <= 65535);

                        if (dst.dt == BE::F32)
                        {
                            Register tmp = BE::getVReg(BE::I32);
                            if (simple)
                            {
                                immMovesPerPred[predId].push_back(BE::AArch64::createMove(
                                    new BE::RegOperand(tmp), new BE::AArch64::ImmeOperand(imm), LOC_STR));
                            }
                            else
                            {
                                auto segments = BE::AArch64::decomposeImm64(
                                    static_cast<unsigned long long>(static_cast<uint32_t>(imm)));
                                bool first = true;
                                for (size_t i = 0; i < 2; ++i)
                                {
                                    if (segments[i] == 0 && !first) continue;
                                    if (first)
                                    {
                                        auto* inst = new BE::AArch64::Instr(BE::AArch64::Operator::MOVZ);
                                        inst->operands.push_back(new BE::RegOperand(tmp));
                                        inst->operands.push_back(new BE::AArch64::ImmeOperand(segments[i]));
                                        if (i > 0) inst->operands.push_back(new BE::AArch64::ImmeOperand(i * 16));
                                        inst->comment = LOC_STR;
                                        immMovesPerPred[predId].push_back(inst);
                                        first = false;
                                    }
                                    else
                                    {
                                        auto* inst = new BE::AArch64::Instr(BE::AArch64::Operator::MOVK);
                                        inst->operands.push_back(new BE::RegOperand(tmp));
                                        inst->operands.push_back(new BE::AArch64::ImmeOperand(segments[i]));
                                        inst->operands.push_back(new BE::AArch64::ImmeOperand(i * 16));
                                        inst->comment = LOC_STR;
                                        immMovesPerPred[predId].push_back(inst);
                                    }
                                }
                            }

                            auto* fmov = new BE::AArch64::Instr(BE::AArch64::Operator::FMOV);
                            fmov->operands.push_back(new BE::RegOperand(dst));
                            fmov->operands.push_back(new BE::RegOperand(tmp));
                            fmov->comment = LOC_STR;
                            immMovesPerPred[predId].push_back(fmov);
                        }
                        else
                        {
                            if (simple)
                            {
                                immMovesPerPred[predId].push_back(BE::AArch64::createMove(
                                    new BE::RegOperand(dst), new BE::AArch64::ImmeOperand(imm), LOC_STR));
                            }
                            else
                            {
                                auto segments = BE::AArch64::decomposeImm64(
                                    static_cast<unsigned long long>(static_cast<uint32_t>(imm)));
                                bool first = true;
                                for (size_t i = 0; i < 2; ++i)
                                {
                                    if (segments[i] == 0 && !first) continue;
                                    if (first)
                                    {
                                        auto* inst = new BE::AArch64::Instr(BE::AArch64::Operator::MOVZ);
                                        inst->operands.push_back(new BE::RegOperand(dst));
                                        inst->operands.push_back(new BE::AArch64::ImmeOperand(segments[i]));
                                        if (i > 0) inst->operands.push_back(new BE::AArch64::ImmeOperand(i * 16));
                                        inst->comment = LOC_STR;
                                        immMovesPerPred[predId].push_back(inst);
                                        first = false;
                                    }
                                    else
                                    {
                                        auto* inst = new BE::AArch64::Instr(BE::AArch64::Operator::MOVK);
                                        inst->operands.push_back(new BE::RegOperand(dst));
                                        inst->operands.push_back(new BE::AArch64::ImmeOperand(segments[i]));
                                        inst->operands.push_back(new BE::AArch64::ImmeOperand(i * 16));
                                        inst->comment = LOC_STR;
                                        immMovesPerPred[predId].push_back(inst);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // For each predecessor edge, insert copies.
            for (uint32_t predId : preds)
            {
                if (!func->blocks.count(predId)) continue;
                auto* predBlock = func->blocks[predId];
                if (!predBlock) continue;

                // If pred has multiple successors, split edge pred->bid.
                bool needSplit = false;
                if (predId < cfg->graph_id.size())
                {
                    // graph_id is indexed by block id.
                    if (cfg->graph_id[predId].size() > 1) needSplit = true;
                }

                BE::Block* insertBlock = predBlock;
                if (needSplit)
                {
                    auto key = std::make_pair(predId, bid);
                    auto it  = edgeSplit.find(key);
                    uint32_t splitId;
                    if (it == edgeSplit.end())
                    {
                        splitId = ++maxId;
                        edgeSplit[key] = splitId;

                        auto* splitBlock = new BE::Block(splitId);
                        func->blocks[splitId] = splitBlock;

                        // Redirect pred's branch target from bid to splitId.
                        rewriteBranchTarget(predBlock, bid, splitId);

                        // split block unconditionally branches to original succ
                        splitBlock->insts.push_back(
                            BE::AArch64::createInstr1(BE::AArch64::Operator::B, new BE::AArch64::LabelOperand(bid)));
                    }
                    else
                    {
                        splitId = it->second;
                    }
                    insertBlock = func->blocks[splitId];
                }

                auto seq = materializeParallelCopies(copiesPerPred[predId]);
                auto& imms = immMovesPerPred[predId];
                seq.insert(seq.end(), imms.begin(), imms.end());

                insertMovesBeforeTerminator(insertBlock, seq, adapter);
            }

            // Remove PHIs from this block.
            std::deque<BE::MInstruction*> rewritten;
            rewritten.clear();
            for (auto* inst : block->insts)
            {
                if (inst && inst->kind == BE::InstKind::PHI)
                {
                    BE::MInstruction::delInst(inst);
                    continue;
                }
                rewritten.push_back(inst);
            }
            block->insts.swap(rewritten);
        }

        delete cfg;
    }
}  // namespace BE::AArch64::Passes::Lowering
