#include <middleend/pass/adce.h>
#include <middleend/module/ir_module.h>
#include <middleend/pass/analysis/analysis_manager.h>
#include <middleend/pass/analysis/cfg.h>
#include <middleend/module/ir_operand.h>
#include <utils/dom_analyzer.h>
#include <algorithm>
#include <iostream>

namespace ME
{
    void ADCEPass::runOnModule(Module& module)
    {
        for (auto* function : module.functions) runOnFunction(*function);
    }

    void ADCEPass::runOnFunction(Function& function)
    {
        // Ensure CFG is built and unreachable blocks are removed
        Analysis::AM.get<Analysis::CFG>(function);

        liveInsts.clear();
        worklist.clear();
        defMap.clear();
        instToBlock.clear();

        buildDefMap(function);

        // Initialize worklist with critical instructions
        // Critical: Side effects (Store, Call, Ret) AND Terminators (to preserve CFG)
        for (auto& [id, block] : function.blocks)
        {
            for (auto* inst : block->insts)
            {
                bool isCritical = false;
                if (inst->isTerminator())
                {
                    isCritical = true;
                }
                else
                {
                    switch (inst->opcode)
                    {
                    case Operator::STORE:
                    case Operator::CALL:
                        isCritical = true;
                        break;
                    default:
                        break;
                    }
                }
                
                if (isCritical)
                {
                    markLive(inst);
                }
            }
        }

        // Propagate liveness
        while (!worklist.empty())
        {
            Instruction* inst = worklist.back();
            worklist.pop_back();

            // Data Dependence
            std::vector<Operand*> uses = getUses(inst);
            for (auto* op : uses)
            {
                Instruction* defInst = getDef(op);
                if (defInst && !isLive(defInst))
                {
                    markLive(defInst);
                }
            }
        }

        // Remove dead instructions
        for (auto& [id, block] : function.blocks)
        {
            auto it = block->insts.begin();
            while (it != block->insts.end())
            {
                Instruction* inst = *it;
                if (!isLive(inst))
                {
                    // Double check it's not a terminator (should be live)
                    if (inst->isTerminator())
                    {
                        // Should not happen if we marked all terminators live
                        ++it;
                    }
                    else
                    {
                        // Remove instruction
                        it = block->insts.erase(it);
                        delete inst;
                    }
                }
                else
                {
                    ++it;
                }
            }
        }
    }

    void ADCEPass::markLive(Instruction* inst)
    {
        if (liveInsts.find(inst) == liveInsts.end())
        {
            liveInsts.insert(inst);
            worklist.push_back(inst);
        }
    }

    bool ADCEPass::isLive(Instruction* inst)
    {
        return liveInsts.find(inst) != liveInsts.end();
    }

    void ADCEPass::buildDefMap(Function& function)
    {
        for (auto& [id, block] : function.blocks)
        {
            for (auto* inst : block->insts)
            {
                instToBlock[inst] = block;
                
                Operand* res = nullptr;
                switch (inst->opcode)
                {
                case Operator::LOAD: res = static_cast<LoadInst*>(inst)->res; break;
                case Operator::ADD:
                case Operator::SUB:
                case Operator::MUL:
                case Operator::DIV:
                case Operator::MOD:
                case Operator::FADD:
                case Operator::FSUB:
                case Operator::FMUL:
                case Operator::FDIV:
                case Operator::BITAND:
                case Operator::BITXOR:
                case Operator::SHL:
                case Operator::LSHR:
                case Operator::ASHR: res = static_cast<ArithmeticInst*>(inst)->res; break;
                case Operator::ICMP: res = static_cast<IcmpInst*>(inst)->res; break;
                case Operator::FCMP: res = static_cast<FcmpInst*>(inst)->res; break;
                case Operator::ALLOCA: res = static_cast<AllocaInst*>(inst)->res; break;
                case Operator::CALL: res = static_cast<CallInst*>(inst)->res; break;
                case Operator::GETELEMENTPTR: res = static_cast<GEPInst*>(inst)->res; break;
                case Operator::SITOFP: res = static_cast<SI2FPInst*>(inst)->dest; break;
                case Operator::FPTOSI: res = static_cast<FP2SIInst*>(inst)->dest; break;
                case Operator::ZEXT: res = static_cast<ZextInst*>(inst)->dest; break;
                case Operator::PHI: res = static_cast<PhiInst*>(inst)->res; break;
                default: break;
                }
                
                if (res && res->getType() == OperandType::REG)
                {
                    defMap[res->getRegNum()] = inst;
                }
            }
        }
    }

    Instruction* ADCEPass::getDef(Operand* op)
    {
        if (op->getType() == OperandType::REG)
        {
            size_t regNum = op->getRegNum();
            if (defMap.count(regNum))
                return defMap[regNum];
        }
        return nullptr;
    }
    
    std::vector<Operand*> ADCEPass::getUses(Instruction* inst)
    {
        std::vector<Operand*> uses;
        switch (inst->opcode)
        {
        case Operator::LOAD:
            uses.push_back(static_cast<LoadInst*>(inst)->ptr);
            break;
        case Operator::STORE:
            uses.push_back(static_cast<StoreInst*>(inst)->val);
            uses.push_back(static_cast<StoreInst*>(inst)->ptr);
            break;
        case Operator::ADD:
        case Operator::SUB:
        case Operator::MUL:
        case Operator::DIV:
        case Operator::MOD:
        case Operator::FADD:
        case Operator::FSUB:
        case Operator::FMUL:
        case Operator::FDIV:
        case Operator::BITAND:
        case Operator::BITXOR:
        case Operator::SHL:
        case Operator::LSHR:
        case Operator::ASHR:
            uses.push_back(static_cast<ArithmeticInst*>(inst)->lhs);
            uses.push_back(static_cast<ArithmeticInst*>(inst)->rhs);
            break;
        case Operator::ICMP:
            uses.push_back(static_cast<IcmpInst*>(inst)->lhs);
            uses.push_back(static_cast<IcmpInst*>(inst)->rhs);
            break;
        case Operator::FCMP:
            uses.push_back(static_cast<FcmpInst*>(inst)->lhs);
            uses.push_back(static_cast<FcmpInst*>(inst)->rhs);
            break;
        case Operator::BR_COND:
            uses.push_back(static_cast<BrCondInst*>(inst)->cond);
            break;
        case Operator::GLOBAL_VAR:
            if (static_cast<GlbVarDeclInst*>(inst)->init)
                 uses.push_back(static_cast<GlbVarDeclInst*>(inst)->init);
            break;
        case Operator::CALL:
            for (auto& arg : static_cast<CallInst*>(inst)->args)
                uses.push_back(arg.second);
            break;
        case Operator::RET:
            if (static_cast<RetInst*>(inst)->res)
                uses.push_back(static_cast<RetInst*>(inst)->res);
            break;
        case Operator::GETELEMENTPTR:
            uses.push_back(static_cast<GEPInst*>(inst)->basePtr);
            for (auto* idx : static_cast<GEPInst*>(inst)->idxs)
                uses.push_back(idx);
            break;
        case Operator::SITOFP:
            uses.push_back(static_cast<SI2FPInst*>(inst)->src);
            break;
        case Operator::FPTOSI:
            uses.push_back(static_cast<FP2SIInst*>(inst)->src);
            break;
        case Operator::ZEXT:
            uses.push_back(static_cast<ZextInst*>(inst)->src);
            break;
        case Operator::PHI:
            for (auto& [label, val] : static_cast<PhiInst*>(inst)->incomingVals)
                uses.push_back(val);
            break;
        default:
            break;
        }
        return uses;
    }

    void ADCEPass::computePostDominance(Function& function, std::map<Block*, std::set<Block*>>& pdf)
    {
        // Not used in this simplified version
    }
}
