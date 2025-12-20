#include <backend/targets/aarch64/aarch64_instr_adapter.h>
#include <backend/targets/aarch64/aarch64_defs.h>
#include <debug.h>

namespace BE::Targeting::AArch64
{
    using namespace BE::AArch64;

    static inline Instr* asInstr(BE::MInstruction* inst)
    {
        return (inst && inst->kind == BE::InstKind::TARGET) ? static_cast<Instr*>(inst) : nullptr;
    }

    bool InstrAdapter::isCall(BE::MInstruction* inst) const
    {
        auto* i = asInstr(inst);
        return i && i->op == Operator::BL;
    }

    bool InstrAdapter::isReturn(BE::MInstruction* inst) const
    {
        auto* i = asInstr(inst);
        return i && i->op == Operator::RET;
    }

    bool InstrAdapter::isUncondBranch(BE::MInstruction* inst) const
    {
        auto* i = asInstr(inst);
        return i && i->op == Operator::B;
    }

    bool InstrAdapter::isCondBranch(BE::MInstruction* inst) const
    {
        auto* i = asInstr(inst);
        if (!i) return false;
        switch (i->op)
        {
            case Operator::BEQ:
            case Operator::BNE:
            case Operator::BLT:
            case Operator::BLE:
            case Operator::BGT:
            case Operator::BGE: return true;
            default: return false;
        }
    }

    int InstrAdapter::extractBranchTarget(BE::MInstruction* inst) const
    {
        auto* i = asInstr(inst);
        if (!i || i->operands.empty()) return -1;
        if (!(isUncondBranch(inst) || isCondBranch(inst))) return -1;
        if (auto* lb = dynamic_cast<LabelOperand*>(i->operands[0])) return lb->targetBlockId;
        return -1;
    }

    void InstrAdapter::enumUses(BE::MInstruction* inst, std::vector<BE::Register>& out) const
    {
        out.clear();

        if (!inst) return;

        if (inst->kind == BE::InstKind::MOVE)
        {
            auto* mv = static_cast<BE::MoveInst*>(inst);
            if (auto* r = dynamic_cast<RegOperand*>(mv->src)) out.push_back(r->reg);
            return;
        }

        if (inst->kind == BE::InstKind::PHI)
        {
            auto* phi = static_cast<BE::PhiInst*>(inst);
            for (auto& [_, op] : phi->incomingVals)
                if (auto* r = dynamic_cast<RegOperand*>(op)) out.push_back(r->reg);
            return;
        }

        if (inst->kind != BE::InstKind::TARGET) return;

        auto* ti = static_cast<Instr*>(inst);
        auto  opt = getOpInfoType(ti->op);

        auto addReg = [&](Operand* op) {
            if (auto* r = dynamic_cast<RegOperand*>(op)) out.push_back(r->reg);
            else if (auto* m = dynamic_cast<MemOperand*>(op)) out.push_back(m->base);
        };

        switch (opt)
        {
            case OpType::R:
                // rd, rs1, rs2 ; uses rs1/rs2
                if (ti->operands.size() >= 2) addReg(ti->operands[1]);
                if (ti->operands.size() >= 3) addReg(ti->operands[2]);
                break;
            case OpType::R2:
                if (ti->op == Operator::CMP || ti->op == Operator::FCMP)
                {
                    if (ti->operands.size() >= 1) addReg(ti->operands[0]);
                    if (ti->operands.size() >= 2) addReg(ti->operands[1]);
                }
                else
                {
                    if (ti->operands.size() >= 2) addReg(ti->operands[1]);
                }
                break;
            case OpType::M:
                if (ti->op == Operator::STR)
                {
                    if (ti->operands.size() >= 1) addReg(ti->operands[0]);
                    if (ti->operands.size() >= 2) addReg(ti->operands[1]);
                }
                else
                {
                    if (ti->operands.size() >= 2) addReg(ti->operands[1]);
                }
                break;
            case OpType::P:
                if (ti->op == Operator::STP)
                {
                    if (ti->operands.size() >= 1) addReg(ti->operands[0]);
                    if (ti->operands.size() >= 2) addReg(ti->operands[1]);
                    if (ti->operands.size() >= 3) addReg(ti->operands[2]);
                }
                else if (ti->op == Operator::LDP)
                {
                    if (ti->operands.size() >= 3) addReg(ti->operands[2]);
                }
                break;
            case OpType::L: break;
            case OpType::SYM: break;
            case OpType::Z: break;
        }
    }

    void InstrAdapter::enumDefs(BE::MInstruction* inst, std::vector<BE::Register>& out) const
    {
        out.clear();
        if (!inst) return;

        if (inst->kind == BE::InstKind::MOVE)
        {
            auto* mv = static_cast<BE::MoveInst*>(inst);
            if (auto* r = dynamic_cast<RegOperand*>(mv->dest)) out.push_back(r->reg);
            return;
        }
        if (inst->kind == BE::InstKind::PHI)
        {
            auto* phi = static_cast<BE::PhiInst*>(inst);
            out.push_back(phi->resReg);
            return;
        }

        if (inst->kind != BE::InstKind::TARGET) return;

        auto* ti  = static_cast<Instr*>(inst);
        auto  opt = getOpInfoType(ti->op);

        auto addReg = [&](Operand* op) {
            if (auto* r = dynamic_cast<RegOperand*>(op)) out.push_back(r->reg);
        };

        switch (opt)
        {
            case OpType::R:
                if (ti->operands.size() >= 1) addReg(ti->operands[0]);
                break;
            case OpType::R2:
                if (ti->op == Operator::CMP || ti->op == Operator::FCMP)
                    ;
                else if (ti->op == Operator::CSET)
                {
                    if (ti->operands.size() >= 1) addReg(ti->operands[0]);
                }
                else
                {
                    if (ti->operands.size() >= 1) addReg(ti->operands[0]);
                }
                break;
            case OpType::M:
                if (ti->op == Operator::LDR)
                {
                    if (ti->operands.size() >= 1) addReg(ti->operands[0]);
                }
                break;
            case OpType::P:
                if (ti->op == Operator::LDP)
                {
                    if (ti->operands.size() >= 1) addReg(ti->operands[0]);
                    if (ti->operands.size() >= 2) addReg(ti->operands[1]);
                }
                break;
            case OpType::L: break;
            case OpType::SYM: break;
            case OpType::Z: break;
        }
    }

    static void replaceOne(Operand* op, const BE::Register& from, const BE::Register& to)
    {
        if (auto* regOp = dynamic_cast<RegOperand*>(op))
            if (regOp->reg == from) regOp->reg = to;
    }

    // tip: 1.MemOperand 的 base 和 RegOperand 的 reg 都是寄存器，需要分别处理
    //      2.注意不同指令的寄存器使用方式不同，个数也不相同，需要根据指令类型进行处理
    //      3.定义寄存器可能为空集，需要根据指令类型进行处理
    //      4.可以借助replaceOne函数实现
    void InstrAdapter::replaceUse(BE::MInstruction* inst, const BE::Register& from, const BE::Register& to) const
    {
        if (!inst) return;

        if (inst->kind == BE::InstKind::MOVE)
        {
            auto* mv = static_cast<BE::MoveInst*>(inst);
            if (mv->src && mv->src->ot == Operand::Type::REG) replaceOne(mv->src, from, to);
            return;
        }
        if (inst->kind == BE::InstKind::PHI)
        {
            auto* phi = static_cast<BE::PhiInst*>(inst);
            for (auto& [lbl, op] : phi->incomingVals)
            {
                (void)lbl;
                replaceOne(op, from, to);
            }
            return;
        }

        if (inst->kind != BE::InstKind::TARGET) return;
        auto* ti  = static_cast<Instr*>(inst);
        auto  opt = getOpInfoType(ti->op);

        auto replMem = [&](Operand* op) {
            if (auto* m = dynamic_cast<MemOperand*>(op))
                if (m->base == from) m->base = to;
        };

        switch (opt)
        {
            case OpType::R:
                if (ti->operands.size() >= 2) replaceOne(ti->operands[1], from, to);
                if (ti->operands.size() >= 3) replaceOne(ti->operands[2], from, to);
                break;
            case OpType::R2:
                if (ti->op == Operator::CMP || ti->op == Operator::FCMP)
                {
                    if (ti->operands.size() >= 1) replaceOne(ti->operands[0], from, to);
                    if (ti->operands.size() >= 2) replaceOne(ti->operands[1], from, to);
                }
                else
                {
                    if (ti->operands.size() >= 2) replaceOne(ti->operands[1], from, to);
                }
                break;
            case OpType::M:
                if (ti->op == Operator::STR)
                {
                    if (ti->operands.size() >= 1) replaceOne(ti->operands[0], from, to);
                    if (ti->operands.size() >= 2) replMem(ti->operands[1]);
                }
                else
                {
                    if (ti->operands.size() >= 2) replMem(ti->operands[1]);
                }
                break;
            case OpType::P:
                if (ti->operands.size() >= 1) replaceOne(ti->operands[0], from, to);
                if (ti->operands.size() >= 2) replaceOne(ti->operands[1], from, to);
                if (ti->operands.size() >= 3) replMem(ti->operands[2]);
                break;
            case OpType::L: break;
            case OpType::SYM: break;
            case OpType::Z: break;
        }
    }

    void InstrAdapter::replaceDef(BE::MInstruction* inst, const BE::Register& from, const BE::Register& to) const
    {
        if (!inst) return;

        if (inst->kind == BE::InstKind::MOVE)
        {
            auto* mv = static_cast<BE::MoveInst*>(inst);
            if (mv->dest && mv->dest->ot == Operand::Type::REG) replaceOne(mv->dest, from, to);
            return;
        }
        if (inst->kind == BE::InstKind::PHI)
        {
            auto* phi = static_cast<BE::PhiInst*>(inst);
            if (phi->resReg == from) phi->resReg = to;
            return;
        }

        if (inst->kind != BE::InstKind::TARGET) return;
        auto* ti  = static_cast<Instr*>(inst);
        auto  opt = getOpInfoType(ti->op);

        switch (opt)
        {
            case OpType::R:
            case OpType::R2:
                if (ti->op == Operator::CMP || ti->op == Operator::FCMP)
                    ;
                else if (ti->operands.size() >= 1)
                    replaceOne(ti->operands[0], from, to);
                break;
            case OpType::M:
                if (ti->op == Operator::LDR && ti->operands.size() >= 1) replaceOne(ti->operands[0], from, to);
                break;
            case OpType::P:
                if (ti->op == Operator::LDP)
                {
                    if (ti->operands.size() >= 1) replaceOne(ti->operands[0], from, to);
                    if (ti->operands.size() >= 2) replaceOne(ti->operands[1], from, to);
                }
                break;
            case OpType::L: break;
            case OpType::SYM: break;
            case OpType::Z: break;
        }
    }

    void InstrAdapter::enumPhysRegs(BE::MInstruction* inst, std::vector<BE::Register>& out) const
    {
        out.clear();
        if (!inst) return;

        auto collectOp = [&](Operand* op) {
            if (auto* r = dynamic_cast<RegOperand*>(op))
                if (!r->reg.isVreg) out.push_back(r->reg);
            if (auto* m = dynamic_cast<MemOperand*>(op))
                if (!m->base.isVreg) out.push_back(m->base);
        };

        if (inst->kind == BE::InstKind::MOVE)
        {
            auto* mv = static_cast<BE::MoveInst*>(inst);
            collectOp(mv->src);
            collectOp(mv->dest);
            return;
        }
        if (inst->kind == BE::InstKind::PHI)
        {
            auto* phi = static_cast<BE::PhiInst*>(inst);
            if (!phi->resReg.isVreg) out.push_back(phi->resReg);
            for (auto& [_, op] : phi->incomingVals) collectOp(op);
            return;
        }
        if (inst->kind != BE::InstKind::TARGET) return;
        auto* ti = static_cast<Instr*>(inst);
        for (auto* op : ti->operands) collectOp(op);
    }

    void InstrAdapter::insertReloadBefore(
        BE::Block* block, std::deque<BE::MInstruction*>::iterator it, const BE::Register& physReg, int frameIndex) const
    {
        auto* reload = new BE::FILoadInst(physReg, frameIndex, LOC_STR);
        block->insts.insert(it, reload);
    }

    void InstrAdapter::insertSpillAfter(
        BE::Block* block, std::deque<BE::MInstruction*>::iterator it, const BE::Register& physReg, int frameIndex) const
    {
        auto next = it;
        ++next;
        auto* spill = new BE::FIStoreInst(physReg, frameIndex, LOC_STR);
        block->insts.insert(next, spill);
    }
}  // namespace BE::Targeting::AArch64
