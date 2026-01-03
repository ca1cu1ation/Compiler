#include <backend/targets/aarch64/passes/lowering/stack_lowering.h>
#include <backend/targets/aarch64/aarch64_defs.h>
#include <backend/targets/aarch64/aarch64_reg_info.h>
#include <backend/mir/m_block.h>
#include <backend/mir/m_function.h>
#include <backend/mir/m_instruction.h>

namespace BE::AArch64::Passes::Lowering
{
    void StackLoweringPass::runOnModule(BE::Module& module)
    {
        for (auto* func : module.functions)
        {
            if (!func) continue;
            lowerFunction(func);
        }
    }

    namespace
    {
        // 重要说明：
        // - FILoadInst/FIStoreInst 的 `frameIndex` 始终是由寄存器分配（RA）创建的溢出槽索引。
        // - FrameIndexOperand/fiop 的 `frameIndex` 用于栈对象（alloca / DAG 帧索引）。
        // 不要试图通过数字自动区分这两者，因为二者的命名空间可能重叠
        // （例如，溢出槽 5 和参数的 IR reg id 5），这会导致静默的栈破坏。

        static int getSpillOffset(const BE::Function* func, int spillIndex)
        {
            if (!func) return -1;
            return func->frameInfo.getSpillSlotOffset(spillIndex);
        }

        static int getObjectOffset(const BE::Function* func, int frameIndex)
        {
            if (!func) return -1;
            if (frameIndex < 0) return -1;
            return func->frameInfo.getObjectOffset(static_cast<size_t>(frameIndex));
        }

        static int getFiOpOffset(const BE::Function* func, int frameIndex)
        {
            int off = getObjectOffset(func, frameIndex);
            if (off >= 0) return off;
            return getSpillOffset(func, frameIndex);
        }

        static inline int regScale(const BE::Register& r)
        {
            if (r.dt == BE::I32 || r.dt == BE::F32) return 4;
            return 8;
        }

        static inline BE::Register pickScratchBase(const BE::Register& avoid)
        {
            // x16/x17 被保留为 RA reload/spill 的 scratch 寄存器。
            // 如果这两个都被用掉（如二元操作有两个溢出输入），则需要第三个 scratch
            // 用于大偏移量的地址计算。
            // 我们在 RegInfo 中为此目的保留了 x15。
            return BE::AArch64::PR::x15;
        }

        static void emitLoadImm64(std::deque<BE::MInstruction*>& out, const BE::Register& dst, unsigned long long value)
        {
            auto segs = BE::AArch64::decomposeImm64(value);
            bool first = true;
            for (size_t i = 0; i < segs.size(); ++i)
            {
                if (!first || segs[i] != 0)
                {
                    if (first)
                    {
                        out.push_back(createInstr2(
                            Operator::MOVZ, new RegOperand(dst), new ImmeOperand(static_cast<int>(segs[i]))));
                        first = false;
                    }
                    else
                    {
                        out.push_back(createInstr3(Operator::MOVK,
                            new RegOperand(dst),
                            new ImmeOperand(static_cast<int>(segs[i])),
                            new ImmeOperand(static_cast<int>(i * 16))));
                    }
                }
            }
            if (first)
            {
                out.push_back(createInstr2(Operator::MOVZ, new RegOperand(dst), new ImmeOperand(0)));
            }
        }

        static void emitSpillAccess(std::deque<BE::MInstruction*>& out,
            Operator memOp,
            const BE::Register& dataReg,
            int off)
        {
            const int scale = regScale(dataReg);

            if (off < 0)
            {
                ERROR("Invalid stack offset %d for %s", off, (memOp == Operator::LDR ? "LDR" : "STR"));
                // 尽力而为：避免汇编器崩溃；但如果执行到这里结果依然不正确。
                off = 0;
            }

            if (BE::AArch64::fitsUnsignedScaledOffset(off, scale))
            {
                out.push_back(createInstr2(memOp, new RegOperand(dataReg), new MemOperand(BE::AArch64::PR::sp, off)));
                return;
            }

            // 大/不可编码的偏移：用 scratch 寄存器计算地址，然后用 [scratch, #0]
            BE::Register scratch = pickScratchBase(dataReg);
            emitLoadImm64(out, scratch, static_cast<unsigned long long>(off));
            out.push_back(createInstr3(Operator::ADD, new RegOperand(scratch), new RegOperand(BE::AArch64::PR::sp), new RegOperand(scratch)));
            out.push_back(createInstr2(memOp, new RegOperand(dataReg), new MemOperand(scratch, 0)));
        }
    }  // namespace

    void StackLoweringPass::lowerFunction(BE::Function* func)
    {
        if (!func) return;

        for (auto& [bid, block] : func->blocks)
        {
            (void)bid;
            if (!block) continue;

            std::deque<BE::MInstruction*> rewritten;
            rewritten.clear();

            for (auto* inst : block->insts)
            {
                // 处理溢出/回填伪指令
                if (auto* fil = dynamic_cast<BE::FILoadInst*>(inst))
                {
                    int off = getSpillOffset(func, fil->frameIndex);
                    emitSpillAccess(rewritten, Operator::LDR, fil->dest, off);
                    BE::MInstruction::delInst(inst);
                    continue;
                }
                if (auto* fis = dynamic_cast<BE::FIStoreInst*>(inst))
                {
                    int off = getSpillOffset(func, fis->frameIndex);
                    emitSpillAccess(rewritten, Operator::STR, fis->src, off);
                    BE::MInstruction::delInst(inst);
                    continue;
                }

                // 目标指令上的 frame index / fiop 消解
                if (inst && inst->kind == BE::InstKind::TARGET)
                {
                    auto* tin = static_cast<Instr*>(inst);

                    // 处理基于 fiop 的地址生成（通常为：ADD rd, sp, FI）
                    if (tin->use_fiops && tin->fiop)
                    {
                        if (auto* fio = dynamic_cast<FrameIndexOperand*>(tin->fiop))
                        {
                            int off = getFiOpOffset(func, fio->frameIndex);
                            if (off < 0)
                            {
                                ERROR("Invalid FI offset for fiop: %d", off);
                                off = 0;
                            }

                            // DAG isel 常见模式：ADD rd, sp, <fiop>
                            if (tin->op == Operator::ADD && tin->operands.size() >= 2)
                            {
                                auto* rdOp = dynamic_cast<RegOperand*>(tin->operands[0]);
                                auto* rsOp = dynamic_cast<RegOperand*>(tin->operands[1]);
                                if (rdOp && rsOp && rsOp->reg.rId == BE::AArch64::PR::sp.rId)
                                {
                                    if (BE::AArch64::fitsUnsignedImm12(off))
                                    {
                                        if (tin->operands.size() == 2) tin->operands.push_back(new ImmeOperand(off));
                                        else if (tin->operands.size() >= 3)
                                        {
                                            delete tin->operands[2];
                                            tin->operands[2] = new ImmeOperand(off);
                                        }
                                        tin->use_fiops = false;
                                    }
                                    else
                                    {
                                        // 展开为：movz/movk rd, off; add rd, sp, rd
                                        BE::Register rd = rdOp->reg;
                                        emitLoadImm64(rewritten, rd, static_cast<unsigned long long>(off));
                                        rewritten.push_back(createInstr3(Operator::ADD,
                                            new RegOperand(rd), new RegOperand(BE::AArch64::PR::sp), new RegOperand(rd)));
                                        BE::MInstruction::delInst(inst);
                                        continue;
                                    }
                                }
                            }
                        }
                        tin->use_fiops = false;
                    }

                    // 替换显式 FrameIndexOperand 为 sp+off
                    for (auto*& op : tin->operands)
                    {
                        if (auto* fio = dynamic_cast<FrameIndexOperand*>(op))
                        {
                            int off = getFiOpOffset(func, fio->frameIndex);
                            delete op;
                            op = new MemOperand(BE::AArch64::PR::sp, off);
                        }
                    }

                    // 合法化 SP 相对的 LDR/STR 指令中不可编码的偏移量。
                    if ((tin->op == Operator::LDR || tin->op == Operator::STR) && tin->operands.size() >= 2)
                    {
                        auto* regOp = dynamic_cast<RegOperand*>(tin->operands[0]);
                        auto* memOp = dynamic_cast<MemOperand*>(tin->operands[1]);
                        if (regOp && memOp && memOp->base.rId == BE::AArch64::PR::sp.rId)
                        {
                            int off = memOp->offset;
                            int scale = regScale(regOp->reg);
                            if (off < 0 || !BE::AArch64::fitsUnsignedScaledOffset(off, scale))
                            {
                                // 替换为基于 scratch 的寻址方式。
                                Operator mem = tin->op;
                                BE::Register data = regOp->reg;
                                emitSpillAccess(rewritten, mem, data, off);
                                BE::MInstruction::delInst(inst);
                                continue;
                            }
                        }
                    }

                    rewritten.push_back(inst);
                    continue;
                }

                rewritten.push_back(inst);
            }

            block->insts.swap(rewritten);
        }
    }
}  // namespace BE::AArch64::Passes::Lowering
