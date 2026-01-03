#include <backend/targets/aarch64/passes/lowering/frame_lowering.h>
#include <backend/targets/aarch64/aarch64_defs.h>
#include <backend/mir/m_block.h>
#include <backend/mir/m_function.h>
#include <debug.h>
#include <algorithm>

namespace BE::AArch64::Passes::Lowering
{
    static bool hasCall(const BE::Function* func)
    {
        if (!func) return false;
        for (auto& [_, block] : func->blocks)
        {
            if (!block) continue;
            for (auto* inst : block->insts)
            {
                if (!inst || inst->kind != BE::InstKind::TARGET) continue;
                auto* tin = static_cast<Instr*>(inst);
                if (tin->op == Operator::BL) return true;
            }
        }
        return false;
    }

    static bool usesCalleeSavedRegs(const BE::Function* func)
    {
        if (!func) return false;
        for (auto& [_, block] : func->blocks)
        {
            if (!block) continue;
            for (auto* inst : block->insts)
            {
                if (!inst || inst->kind != BE::InstKind::TARGET) continue;
                auto* tin = static_cast<Instr*>(inst);
                for (auto* op : tin->operands)
                {
                    if (op->ot == BE::Operand::Type::REG)
                    {
                        auto* regOp = static_cast<RegOperand*>(op);
                        int id = regOp->reg.rId;
                        if ((id >= A64_CALLEE_INT_FIRST && id <= A64_CALLEE_INT_LAST) ||
                            (id >= A64_CALLEE_FP_FIRST && id <= A64_CALLEE_FP_LAST))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    void FrameLoweringPass::runOnModule(BE::Module& module)
    {
        for (auto* func : module.functions)
        {
            if (!func) continue;
            lowerFunction(func);
        }
    }

    void FrameLoweringPass::lowerFunction(BE::Function* func)
    {
        if (!func) return;

        // 计算帧对象偏移与总栈大小（16 字节对齐）
        func->stackSize = func->frameInfo.calculateOffsets();
        int stackSize   = func->stackSize;
        if (stackSize % 16 != 0) stackSize = (stackSize + 15) & ~15;
        func->stackSize = stackSize;

        if (func->blocks.empty()) return;

        auto* entryBlock = func->blocks.begin()->second;
        if (!entryBlock) return;

        // AArch64 ABI:
        // - 非叶子函数必须保存/恢复 LR(x30)
        // - 被调用者保存寄存器：x19-x28, d8-d15（为了简单与稳妥，这里统一保存一整组）
        // - 栈需要 16 字节对齐
        const bool needPrologue = hasCall(func) || stackSize > 0 || func->hasStackParam || usesCalleeSavedRegs(func);
        if (needPrologue)
        {
            std::deque<BE::MInstruction*> prologue;

            // 压入被调用者保存的浮点寄存器 d8-d15（4 对）
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::d8), new RegOperand(PR::d9), new RegOperand(PR::sp), new ImmeOperand(-16)));
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::d10), new RegOperand(PR::d11), new RegOperand(PR::sp), new ImmeOperand(-16)));
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::d12), new RegOperand(PR::d13), new RegOperand(PR::sp), new ImmeOperand(-16)));
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::d14), new RegOperand(PR::d15), new RegOperand(PR::sp), new ImmeOperand(-16)));

            // 压入被调用者保存的通用寄存器 x19-x28（5 对）
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::x19), new RegOperand(PR::x20), new RegOperand(PR::sp), new ImmeOperand(-16)));
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::x21), new RegOperand(PR::x22), new RegOperand(PR::sp), new ImmeOperand(-16)));
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::x23), new RegOperand(PR::x24), new RegOperand(PR::sp), new ImmeOperand(-16)));
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::x25), new RegOperand(PR::x26), new RegOperand(PR::sp), new ImmeOperand(-16)));
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::x27), new RegOperand(PR::x28), new RegOperand(PR::sp), new ImmeOperand(-16)));

            // 压入帧指针和返回地址，建立帧指针
            prologue.push_back(
                createInstr4(Operator::STP, new RegOperand(PR::x29), new RegOperand(PR::x30), new RegOperand(PR::sp), new ImmeOperand(-16)));
            prologue.push_back(BE::AArch64::createMove(new RegOperand(PR::x29), new RegOperand(PR::sp)));

            // 分配本地栈空间
            if (stackSize > 0)
            {
                // AArch64 的加/减立即数有限制；通过分块保证 SP 始终 16 字节对齐。
                int remaining = stackSize;
                while (remaining > 0)
                {
                    int chunk = std::min(remaining, 4080);  // <= 4095 且 16 字节对齐
                    prologue.push_back(createInstr3(
                        Operator::SUB, new RegOperand(PR::sp), new RegOperand(PR::sp), new ImmeOperand(chunk)));
                    remaining -= chunk;
                }
            }

            entryBlock->insts.insert(entryBlock->insts.begin(), prologue.begin(), prologue.end());
        }

        // 在所有 RET 指令前恢复寄存器现场
        for (auto& [bid, block] : func->blocks)
        {
            (void)bid;
            std::deque<BE::MInstruction*> rewritten;
            rewritten.resize(0);
            rewritten.clear();
            rewritten.insert(rewritten.end(), block->insts.begin(), block->insts.end());

            if (!needPrologue) continue;

            rewritten.clear();
            for (auto* inst : block->insts)
            {
                if (inst && inst->kind == BE::InstKind::TARGET)
                {
                    auto* tin = static_cast<Instr*>(inst);
                    if (tin->op == Operator::RET)
                    {
                        if (stackSize > 0)
                        {
                            int remaining = stackSize;
                            while (remaining > 0)
                            {
                                int chunk = std::min(remaining, 4080);  // <= 4095 and 16-byte aligned
                                rewritten.push_back(createInstr3(
                                    Operator::ADD, new RegOperand(PR::sp), new RegOperand(PR::sp), new ImmeOperand(chunk)));
                                remaining -= chunk;
                            }
                        }

                        // 按压栈顺序逆序弹出（栈顶保存 fp/lr）
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::x29), new RegOperand(PR::x30), new RegOperand(PR::sp), new ImmeOperand(16)));

                        // 恢复被调用者保存的通用寄存器
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::x27), new RegOperand(PR::x28), new RegOperand(PR::sp), new ImmeOperand(16)));
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::x25), new RegOperand(PR::x26), new RegOperand(PR::sp), new ImmeOperand(16)));
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::x23), new RegOperand(PR::x24), new RegOperand(PR::sp), new ImmeOperand(16)));
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::x21), new RegOperand(PR::x22), new RegOperand(PR::sp), new ImmeOperand(16)));
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::x19), new RegOperand(PR::x20), new RegOperand(PR::sp), new ImmeOperand(16)));

                        // 恢复被调用者保存的浮点寄存器
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::d14), new RegOperand(PR::d15), new RegOperand(PR::sp), new ImmeOperand(16)));
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::d12), new RegOperand(PR::d13), new RegOperand(PR::sp), new ImmeOperand(16)));
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::d10), new RegOperand(PR::d11), new RegOperand(PR::sp), new ImmeOperand(16)));
                        rewritten.push_back(
                            createInstr4(Operator::LDP, new RegOperand(PR::d8), new RegOperand(PR::d9), new RegOperand(PR::sp), new ImmeOperand(16)));
                    }
                }
                rewritten.push_back(inst);
            }
            block->insts.swap(rewritten);
        }
    }
}  // namespace BE::AArch64::Passes::Lowering
