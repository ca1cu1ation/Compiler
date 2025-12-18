#include <middleend/pass/sccp.h>
#include <middleend/module/ir_module.h>
#include <middleend/pass/analysis/analysis_manager.h>
#include <middleend/pass/analysis/cfg.h>
#include <middleend/module/ir_operand.h>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace ME
{
    // 比较两个格值是否相等
    // 原理：类型不同直接不等，常量则比较常量值，否则只要类型相同即相等
    bool SCCPPass::LatticeVal::operator==(const LatticeVal& other) const {
        if (type != other.type) return false;
        if (type == CONSTANT) {
            if (val == other.val) return true;
            if (!val || !other.val) return false;
            if (val->getType() != other.val->getType()) return false;
            if (val->getType() == OperandType::IMMEI32) {
                return static_cast<ImmeI32Operand*>(val)->value == static_cast<ImmeI32Operand*>(other.val)->value;
            } else if (val->getType() == OperandType::IMMEF32) {
                return static_cast<ImmeF32Operand*>(val)->value == static_cast<ImmeF32Operand*>(other.val)->value;
            }
            return val == other.val;
        }
        return true;
    }

    // 对整个模块进行SCCP优化，每个函数单独处理
    void SCCPPass::runOnModule(Module& module)
    {
        for (auto* function : module.functions) runOnFunction(*function);
    }

    // 对单个函数进行SCCP优化
    // 原理：先初始化格值和可达块，然后不断处理流敏感和SSA工作队列，直到收敛，最后替换常量
    void SCCPPass::runOnFunction(Function& function)
    {
        Analysis::AM.get<Analysis::CFG>(function);
        currentFunction = &function;
        
        initialize(function);
        
        if (!function.blocks.empty()) {
            // 从入口块开始标记可达
            markBlockExecutable(function.blocks.begin()->second);
        }
        
        // 主循环：处理流敏感和SSA工作队列
        while (!flowWorklist.empty() || !ssaWorklist.empty()) {
            while (!flowWorklist.empty()) {
                auto [pred, succ] = flowWorklist.front();
                flowWorklist.pop();
                
                size_t predId = pred ? pred->blockId : -1;
                size_t succId = succ->blockId;
                
                if (executableEdges.count({predId, succId})) continue;
                executableEdges.insert({predId, succId});
                
                if (executableBlocks.find(succId) == executableBlocks.end()) {
                    markBlockExecutable(succ);
                } else {
                    for (auto* inst : succ->insts) {
                        if (inst->opcode == Operator::PHI) {
                            visitPhi(static_cast<PhiInst*>(inst));
                        }
                    }
                }
            }
            
            while (!ssaWorklist.empty()) {
                Instruction* inst = ssaWorklist.front();
                ssaWorklist.pop();
                
                if (inst->opcode == Operator::PHI) {
                    visitPhi(static_cast<PhiInst*>(inst));
                } else {
                    visitInstruction(inst);
                }
            }
        }
        
        // 替换常量传播结果
        replaceConstants(function);
    }

    // 初始化SCCP状态，包括格值、可达块、工作队列等
    void SCCPPass::initialize(Function& function)
    {
        latticeValues.clear();
        executableBlocks.clear();
        executableEdges.clear();
        instBlockMap.clear();
        while(!flowWorklist.empty()) flowWorklist.pop();
        while(!ssaWorklist.empty()) ssaWorklist.pop();
        
        buildUseMap(function);
        
        // 参数寄存器一律标记为overdefined（不可常量传播）
        if (function.funcDef) {
            for (auto& arg : function.funcDef->argRegs) {
                if (arg.second && arg.second->getType() == OperandType::REG) {
                    markOverdefined(arg.second);
                }
            }
        }
    }

    // 构建useMap，记录每个寄存器的所有使用者指令
    void SCCPPass::buildUseMap(Function& function)
    {
        useMap.clear();
        for (auto& [id, block] : function.blocks) {
            for (auto* inst : block->insts) {
                instBlockMap[inst] = block;
                
                auto addUse = [&](Operand* op) {
                    if (op && op->getType() == OperandType::REG) {
                        useMap[op->getRegNum()].push_back(inst);
                    }
                };
                
                // 针对不同指令类型，收集其用到的寄存器
                switch (inst->opcode) {
                    case Operator::ADD: case Operator::SUB: case Operator::MUL: case Operator::DIV:
                    case Operator::MOD: case Operator::FADD: case Operator::FSUB: case Operator::FMUL:
                    case Operator::FDIV: case Operator::BITAND: case Operator::BITXOR: case Operator::SHL:
                    case Operator::LSHR: case Operator::ASHR:
                        addUse(static_cast<ArithmeticInst*>(inst)->lhs);
                        addUse(static_cast<ArithmeticInst*>(inst)->rhs);
                        break;
                    case Operator::ICMP:
                        addUse(static_cast<IcmpInst*>(inst)->lhs);
                        addUse(static_cast<IcmpInst*>(inst)->rhs);
                        break;
                    case Operator::FCMP:
                        addUse(static_cast<FcmpInst*>(inst)->lhs);
                        addUse(static_cast<FcmpInst*>(inst)->rhs);
                        break;
                    case Operator::LOAD:
                        addUse(static_cast<LoadInst*>(inst)->ptr);
                        break;
                    case Operator::STORE:
                        addUse(static_cast<StoreInst*>(inst)->val);
                        addUse(static_cast<StoreInst*>(inst)->ptr);
                        break;
                    case Operator::BR_COND:
                        addUse(static_cast<BrCondInst*>(inst)->cond);
                        break;
                    case Operator::CALL:
                        for (auto& arg : static_cast<CallInst*>(inst)->args) addUse(arg.second);
                        break;
                    case Operator::RET:
                        addUse(static_cast<RetInst*>(inst)->res);
                        break;
                    case Operator::GETELEMENTPTR:
                        addUse(static_cast<GEPInst*>(inst)->basePtr);
                        for (auto* idx : static_cast<GEPInst*>(inst)->idxs) addUse(idx);
                        break;
                    case Operator::SITOFP:
                        addUse(static_cast<SI2FPInst*>(inst)->src);
                        break;
                    case Operator::FPTOSI:
                        addUse(static_cast<FP2SIInst*>(inst)->src);
                        break;
                    case Operator::ZEXT:
                        addUse(static_cast<ZextInst*>(inst)->src);
                        break;
                    case Operator::PHI:
                        for (auto& [lbl, val] : static_cast<PhiInst*>(inst)->incomingVals) addUse(val);
                        break;
                    default: break;
                }
            }
        }
    }

    // 标记一个块为可达，并处理其所有指令
    void SCCPPass::markBlockExecutable(Block* block)
    {
        if (executableBlocks.count(block->blockId)) return;
        executableBlocks.insert(block->blockId);
        
        for (auto* inst : block->insts) {
            if (inst->opcode == Operator::PHI) {
                visitPhi(static_cast<PhiInst*>(inst));
            } else {
                visitInstruction(inst);
            }
        }
    }

    // 标记一条边为可达，加入流敏感工作队列
    void SCCPPass::markEdgeExecutable(Block* pred, Block* succ)
    {
        flowWorklist.push({pred, succ});
    }

    // 处理Phi指令，合并所有可达前驱的格值
    // 原理：只考虑可达边，遇到不同常量则为overdefined，否则合并常量
    void SCCPPass::visitPhi(PhiInst* phi)
    {
        LatticeVal merged = {TOP, nullptr};
        Block* block = instBlockMap[phi];
        
        for (auto& [label, val] : phi->incomingVals) {
            size_t predId = static_cast<LabelOperand*>(label)->lnum;
            if (executableEdges.count({predId, block->blockId})) {
                LatticeVal v = getValueState(val);
                if (v.type == BOTTOM) {
                    merged = {BOTTOM, nullptr};
                    break;
                }
                if (v.type == CONSTANT) {
                    if (merged.type == TOP) {
                        merged = v;
                    } else if (merged.type == CONSTANT) {
                        if (!areConstantsEqual(merged.val, v.val)) {
                            merged = {BOTTOM, nullptr};
                            break;
                        }
                    }
                }
            }
        }
        
        if (merged.type == CONSTANT) markConstant(phi->res, merged.val);
        else if (merged.type == BOTTOM) markOverdefined(phi->res);
    }

    // 处理非Phi指令，包括分支、普通运算等
    // 原理：分支指令根据条件格值决定可达边，普通指令做常量折叠
    void SCCPPass::visitInstruction(Instruction* inst)
    {
        Block* block = instBlockMap[inst];
        
        if (inst->isTerminator()) {
            if (inst->opcode == Operator::BR_COND) {
                BrCondInst* br = static_cast<BrCondInst*>(inst);
                LatticeVal condVal = getValueState(br->cond);
                
                if (condVal.type == CONSTANT) {
                    ImmeI32Operand* imm = static_cast<ImmeI32Operand*>(condVal.val);
                    if (imm->value != 0) {
                        size_t targetId = static_cast<LabelOperand*>(br->trueTar)->lnum;
                        markEdgeExecutable(block, currentFunction->blocks[targetId]);
                    } else {
                        size_t targetId = static_cast<LabelOperand*>(br->falseTar)->lnum;
                        markEdgeExecutable(block, currentFunction->blocks[targetId]);
                    }
                } else if (condVal.type == BOTTOM) {
                    size_t trueId = static_cast<LabelOperand*>(br->trueTar)->lnum;
                    size_t falseId = static_cast<LabelOperand*>(br->falseTar)->lnum;
                    markEdgeExecutable(block, currentFunction->blocks[trueId]);
                    markEdgeExecutable(block, currentFunction->blocks[falseId]);
                }
            } else if (inst->opcode == Operator::BR_UNCOND) {
                BrUncondInst* br = static_cast<BrUncondInst*>(inst);
                size_t targetId = static_cast<LabelOperand*>(br->target)->lnum;
                markEdgeExecutable(block, currentFunction->blocks[targetId]);
            }
        }
        
        if (inst->opcode == Operator::PHI) return; // Handled separately
        
        LatticeVal result = evaluate(inst);
        Operand* resOp = nullptr;
        
        switch(inst->opcode) {
            case Operator::ADD: case Operator::SUB: case Operator::MUL: case Operator::DIV:
            case Operator::MOD: case Operator::FADD: case Operator::FSUB: case Operator::FMUL:
            case Operator::FDIV: case Operator::BITAND: case Operator::BITXOR: case Operator::SHL:
            case Operator::LSHR: case Operator::ASHR:
                resOp = static_cast<ArithmeticInst*>(inst)->res; break;
            case Operator::ICMP: resOp = static_cast<IcmpInst*>(inst)->res; break;
            case Operator::FCMP: resOp = static_cast<FcmpInst*>(inst)->res; break;
            case Operator::SITOFP: resOp = static_cast<SI2FPInst*>(inst)->dest; break;
            case Operator::FPTOSI: resOp = static_cast<FP2SIInst*>(inst)->dest; break;
            case Operator::ZEXT: resOp = static_cast<ZextInst*>(inst)->dest; break;
            case Operator::LOAD: resOp = static_cast<LoadInst*>(inst)->res; break;
            case Operator::CALL: resOp = static_cast<CallInst*>(inst)->res; break;
            case Operator::GETELEMENTPTR: resOp = static_cast<GEPInst*>(inst)->res; break;
            default: break;
        }
        
        if (resOp) {
            if (result.type == CONSTANT) markConstant(resOp, result.val);
            else if (result.type == BOTTOM) markOverdefined(resOp);
        }
    }

    // 对一条指令做常量折叠，返回其格值
    SCCPPass::LatticeVal SCCPPass::evaluate(Instruction* inst)
    {
        switch (inst->opcode) {
            case Operator::ADD: case Operator::SUB: case Operator::MUL: case Operator::DIV:
            case Operator::MOD: case Operator::FADD: case Operator::FSUB: case Operator::FMUL:
            case Operator::FDIV: case Operator::BITAND: case Operator::BITXOR: case Operator::SHL:
            case Operator::LSHR: case Operator::ASHR: {
                ArithmeticInst* arith = static_cast<ArithmeticInst*>(inst);
                LatticeVal lhs = getValueState(arith->lhs);
                LatticeVal rhs = getValueState(arith->rhs);
                if (lhs.type == BOTTOM || rhs.type == BOTTOM) return {BOTTOM, nullptr};
                if (lhs.type == CONSTANT && rhs.type == CONSTANT) {
                    Operand* res = computeBinary(inst->opcode, lhs.val, rhs.val);
                    if (res) return {CONSTANT, res};
                    return {BOTTOM, nullptr};
                }
                return {TOP, nullptr};
            }
            case Operator::ICMP: {
                IcmpInst* icmp = static_cast<IcmpInst*>(inst);
                LatticeVal lhs = getValueState(icmp->lhs);
                LatticeVal rhs = getValueState(icmp->rhs);
                if (lhs.type == BOTTOM || rhs.type == BOTTOM) return {BOTTOM, nullptr};
                if (lhs.type == CONSTANT && rhs.type == CONSTANT) {
                    Operand* res = computeIcmp(icmp->cond, lhs.val, rhs.val);
                    if (res) return {CONSTANT, res};
                    return {BOTTOM, nullptr};
                }
                return {TOP, nullptr};
            }
            case Operator::FCMP: {
                FcmpInst* fcmp = static_cast<FcmpInst*>(inst);
                LatticeVal lhs = getValueState(fcmp->lhs);
                LatticeVal rhs = getValueState(fcmp->rhs);
                if (lhs.type == BOTTOM || rhs.type == BOTTOM) return {BOTTOM, nullptr};
                if (lhs.type == CONSTANT && rhs.type == CONSTANT) {
                    Operand* res = computeFcmp(fcmp->cond, lhs.val, rhs.val);
                    if (res) return {CONSTANT, res};
                    return {BOTTOM, nullptr};
                }
                return {TOP, nullptr};
            }
            case Operator::SITOFP: {
                SI2FPInst* cast = static_cast<SI2FPInst*>(inst);
                LatticeVal src = getValueState(cast->src);
                if (src.type == BOTTOM) return {BOTTOM, nullptr};
                if (src.type == CONSTANT) {
                    ImmeI32Operand* imm = static_cast<ImmeI32Operand*>(src.val);
                    return {CONSTANT, getImmeF32Operand((float)imm->value)};
                }
                return {TOP, nullptr};
            }
            case Operator::FPTOSI: {
                FP2SIInst* cast = static_cast<FP2SIInst*>(inst);
                LatticeVal src = getValueState(cast->src);
                if (src.type == BOTTOM) return {BOTTOM, nullptr};
                if (src.type == CONSTANT) {
                    ImmeF32Operand* imm = static_cast<ImmeF32Operand*>(src.val);
                    return {CONSTANT, getImmeI32Operand((int)imm->value)};
                }
                return {TOP, nullptr};
            }
            case Operator::ZEXT: {
                ZextInst* cast = static_cast<ZextInst*>(inst);
                LatticeVal src = getValueState(cast->src);
                if (src.type == BOTTOM) return {BOTTOM, nullptr};
                if (src.type == CONSTANT) {
                    // Assuming ZEXT is I1/I8 -> I32, and ImmeI32Operand handles it
                    return {CONSTANT, src.val};
                }
                return {TOP, nullptr};
            }
            default:
                return {BOTTOM, nullptr};
        }
    }

    // 获取一个操作数的格值（常量/未知/overdefined）
    SCCPPass::LatticeVal SCCPPass::getValueState(Operand* op)
    {
        if (op->getType() == OperandType::IMMEI32 || op->getType() == OperandType::IMMEF32) {
            return {CONSTANT, op};
        }
        if (op->getType() == OperandType::REG) {
            size_t reg = op->getRegNum();
            if (latticeValues.find(reg) == latticeValues.end()) {
                return {TOP, nullptr};
            }
            return latticeValues[reg];
        }
        return {BOTTOM, nullptr};
    }

    // 标记一个寄存器为overdefined，并将其所有使用者加入SSA工作队列
    void SCCPPass::markOverdefined(Operand* op)
    {
        if (op->getType() != OperandType::REG) return;
        size_t reg = op->getRegNum();
        
        LatticeVal& state = latticeValues[reg];
        if (state.type != BOTTOM) {
            state = {BOTTOM, nullptr};
            for (auto* user : useMap[reg]) {
                ssaWorklist.push(user);
            }
        }
    }

    // 标记一个寄存器为常量，并将其所有使用者加入SSA工作队列
    void SCCPPass::markConstant(Operand* op, Operand* constant)
    {
        if (op->getType() != OperandType::REG) return;
        size_t reg = op->getRegNum();
        
        LatticeVal& state = latticeValues[reg];
        if (state.type == TOP) {
            state = {CONSTANT, constant};
            for (auto* user : useMap[reg]) {
                ssaWorklist.push(user);
            }
        } else if (state.type == CONSTANT && !areConstantsEqual(state.val, constant)) {
            markOverdefined(op);
        }
    }

    // 判断两个常量操作数是否相等
    bool SCCPPass::areConstantsEqual(Operand* op1, Operand* op2)
    {
        if (op1->getType() != op2->getType()) return false;
        if (op1->getType() == OperandType::IMMEI32) {
            return static_cast<ImmeI32Operand*>(op1)->value == static_cast<ImmeI32Operand*>(op2)->value;
        } else if (op1->getType() == OperandType::IMMEF32) {
            return static_cast<ImmeF32Operand*>(op1)->value == static_cast<ImmeF32Operand*>(op2)->value;
        }
        return false;
    }

    // 对二元运算指令做常量折叠
    Operand* SCCPPass::computeBinary(Operator op, Operand* lhs, Operand* rhs)
    {
        if (lhs->getType() == OperandType::IMMEI32 && rhs->getType() == OperandType::IMMEI32) {
            int l = static_cast<ImmeI32Operand*>(lhs)->value;
            int r = static_cast<ImmeI32Operand*>(rhs)->value;
            int res = 0;
            switch(op) {
                case Operator::ADD: res = l + r; break;
                case Operator::SUB: res = l - r; break;
                case Operator::MUL: res = l * r; break;
                case Operator::DIV: if(r==0) return nullptr; res = l / r; break;
                case Operator::MOD: if(r==0) return nullptr; res = l % r; break;
                case Operator::BITAND: res = l & r; break;
                case Operator::BITXOR: res = l ^ r; break;
                case Operator::SHL: res = l << r; break;
                case Operator::LSHR: res = (unsigned)l >> r; break;
                case Operator::ASHR: res = l >> r; break;
                default: return nullptr;
            }
            return getImmeI32Operand(res);
        } else if (lhs->getType() == OperandType::IMMEF32 && rhs->getType() == OperandType::IMMEF32) {
            float l = static_cast<ImmeF32Operand*>(lhs)->value;
            float r = static_cast<ImmeF32Operand*>(rhs)->value;
            float res = 0;
            switch(op) {
                case Operator::FADD: res = l + r; break;
                case Operator::FSUB: res = l - r; break;
                case Operator::FMUL: res = l * r; break;
                case Operator::FDIV: res = l / r; break;
                default: return nullptr;
            }
            return getImmeF32Operand(res);
        }
        return nullptr;
    }

    // 对整数比较指令做常量折叠
    Operand* SCCPPass::computeIcmp(ICmpOp op, Operand* lhs, Operand* rhs)
    {
        if (lhs->getType() == OperandType::IMMEI32 && rhs->getType() == OperandType::IMMEI32) {
            int l = static_cast<ImmeI32Operand*>(lhs)->value;
            int r = static_cast<ImmeI32Operand*>(rhs)->value;
            bool res = false;
            switch(op) {
                case ICmpOp::EQ: res = l == r; break;
                case ICmpOp::NE: res = l != r; break;
                case ICmpOp::SGT: res = l > r; break;
                case ICmpOp::SGE: res = l >= r; break;
                case ICmpOp::SLT: res = l < r; break;
                case ICmpOp::SLE: res = l <= r; break;
                default: return nullptr;
            }
            return getImmeI32Operand(res ? 1 : 0);
        }
        return nullptr;
    }

    // 对浮点比较指令做常量折叠
    Operand* SCCPPass::computeFcmp(FCmpOp op, Operand* lhs, Operand* rhs)
    {
        if (lhs->getType() == OperandType::IMMEF32 && rhs->getType() == OperandType::IMMEF32) {
            float l = static_cast<ImmeF32Operand*>(lhs)->value;
            float r = static_cast<ImmeF32Operand*>(rhs)->value;
            bool res = false;
            switch(op) {
                case FCmpOp::OEQ: case FCmpOp::UEQ: res = l == r; break;
                case FCmpOp::ONE: case FCmpOp::UNE: res = l != r; break;
                case FCmpOp::OGT: case FCmpOp::UGT: res = l > r; break;
                case FCmpOp::OGE: case FCmpOp::UGE: res = l >= r; break;
                case FCmpOp::OLT: case FCmpOp::ULT: res = l < r; break;
                case FCmpOp::OLE: case FCmpOp::ULE: res = l <= r; break;
                default: return nullptr;
            }
            return getImmeI32Operand(res ? 1 : 0);
        }
        return nullptr;
    }

    // 替换函数中的常量传播结果，将可确定的常量直接替换到指令中
    bool SCCPPass::replaceConstants(Function& function)
    {
        bool changed = false;
        for (auto& [id, block] : function.blocks) {
            auto it = block->insts.begin();
            while (it != block->insts.end()) {
                Instruction* inst = *it;
                
                // 替换分支指令（如果条件为常量则转为无条件跳转）
                if (inst->opcode == Operator::BR_COND) {
                    BrCondInst* br = static_cast<BrCondInst*>(inst);
                    LatticeVal condVal = getValueState(br->cond);
                    if (condVal.type == CONSTANT) {
                        ImmeI32Operand* imm = static_cast<ImmeI32Operand*>(condVal.val);
                        bool isTrue = (imm->value != 0);
                        Operand* target = isTrue ? br->trueTar : br->falseTar;
                        Operand* notTaken = isTrue ? br->falseTar : br->trueTar;
                        
                        // 更新未被跳转到的块中的Phi节点
                        size_t notTakenId = static_cast<LabelOperand*>(notTaken)->lnum;
                        if (function.blocks.count(notTakenId)) {
                            Block* notTakenBlock = function.blocks[notTakenId];
                            Operand* currentLabel = getLabelOperand(block->blockId);
                            
                            for (auto* i : notTakenBlock->insts) {
                                if (i->opcode == Operator::PHI) {
                                    PhiInst* phi = static_cast<PhiInst*>(i);
                                    phi->incomingVals.erase(currentLabel);
                                } else {
                                    break; 
                                }
                            }
                        }

                        *it = new BrUncondInst(target);
                        delete inst;
                        changed = true;
                        it++;
                        continue;
                    }
                }
                
                // Replace Uses
                Operand* resOp = nullptr;
                switch(inst->opcode) {
                    case Operator::ADD: case Operator::SUB: case Operator::MUL: case Operator::DIV:
                    case Operator::MOD: case Operator::FADD: case Operator::FSUB: case Operator::FMUL:
                    case Operator::FDIV: case Operator::BITAND: case Operator::BITXOR: case Operator::SHL:
                    case Operator::LSHR: case Operator::ASHR:
                        resOp = static_cast<ArithmeticInst*>(inst)->res; break;
                    case Operator::ICMP: resOp = static_cast<IcmpInst*>(inst)->res; break;
                    case Operator::FCMP: resOp = static_cast<FcmpInst*>(inst)->res; break;
                    case Operator::SITOFP: resOp = static_cast<SI2FPInst*>(inst)->dest; break;
                    case Operator::FPTOSI: resOp = static_cast<FP2SIInst*>(inst)->dest; break;
                    case Operator::ZEXT: resOp = static_cast<ZextInst*>(inst)->dest; break;
                    case Operator::LOAD: resOp = static_cast<LoadInst*>(inst)->res; break;
                    case Operator::CALL: resOp = static_cast<CallInst*>(inst)->res; break;
                    case Operator::GETELEMENTPTR: resOp = static_cast<GEPInst*>(inst)->res; break;
                    case Operator::PHI: resOp = static_cast<PhiInst*>(inst)->res; break;
                    default: break;
                }
                
                if (resOp && resOp->getType() == OperandType::REG) {
                    LatticeVal val = getValueState(resOp);
                    if (val.type == CONSTANT) {
                        // 需要将resOp的所有使用都替换为val.val
                        // 由于没有全局的replaceUsesWith，这里遍历所有使用者
                        size_t reg = resOp->getRegNum();
                        for (auto* user : useMap[reg]) {
                            // 辅助函数：替换指令中的操作数
                            auto replaceOp = [&](Operand*& op) {
                                if (op && op->getType() == OperandType::REG && op->getRegNum() == reg) {
                                    op = val.val;
                                }
                            };
                            
                            switch (user->opcode) {
                                case Operator::ADD: case Operator::SUB: case Operator::MUL: case Operator::DIV:
                                case Operator::MOD: case Operator::FADD: case Operator::FSUB: case Operator::FMUL:
                                case Operator::FDIV: case Operator::BITAND: case Operator::BITXOR: case Operator::SHL:
                                case Operator::LSHR: case Operator::ASHR:
                                    replaceOp(static_cast<ArithmeticInst*>(user)->lhs);
                                    replaceOp(static_cast<ArithmeticInst*>(user)->rhs);
                                    break;
                                case Operator::ICMP:
                                    replaceOp(static_cast<IcmpInst*>(user)->lhs);
                                    replaceOp(static_cast<IcmpInst*>(user)->rhs);
                                    break;
                                case Operator::FCMP:
                                    replaceOp(static_cast<FcmpInst*>(user)->lhs);
                                    replaceOp(static_cast<FcmpInst*>(user)->rhs);
                                    break;
                                case Operator::LOAD:
                                    replaceOp(static_cast<LoadInst*>(user)->ptr);
                                    break;
                                case Operator::STORE:
                                    replaceOp(static_cast<StoreInst*>(user)->val);
                                    replaceOp(static_cast<StoreInst*>(user)->ptr);
                                    break;
                                case Operator::BR_COND:
                                    replaceOp(static_cast<BrCondInst*>(user)->cond);
                                    break;
                                case Operator::CALL:
                                    for (auto& arg : static_cast<CallInst*>(user)->args) replaceOp(arg.second);
                                    break;
                                case Operator::RET:
                                    replaceOp(static_cast<RetInst*>(user)->res);
                                    break;
                                case Operator::GETELEMENTPTR:
                                    replaceOp(static_cast<GEPInst*>(user)->basePtr);
                                    for (auto* idx : static_cast<GEPInst*>(user)->idxs) replaceOp(idx);
                                    break;
                                case Operator::SITOFP:
                                    replaceOp(static_cast<SI2FPInst*>(user)->src);
                                    break;
                                case Operator::FPTOSI:
                                    replaceOp(static_cast<FP2SIInst*>(user)->src);
                                    break;
                                case Operator::ZEXT:
                                    replaceOp(static_cast<ZextInst*>(user)->src);
                                    break;
                                case Operator::PHI:
                                    for (auto& [lbl, v] : static_cast<PhiInst*>(user)->incomingVals) replaceOp(v);
                                    break;
                                default: break;
                            }
                        }
                        // 指令本身已无用，但留给ADCE删除
                        changed = true;
                    }
                }
                
                it++;
            }
        }
        return changed;
    }
}
