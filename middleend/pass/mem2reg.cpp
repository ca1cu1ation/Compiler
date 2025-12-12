#include <middleend/pass/mem2reg.h>
#include <middleend/module/ir_module.h>
#include <middleend/pass/analysis/analysis_manager.h>
#include <middleend/pass/analysis/cfg.h>
#include <middleend/module/ir_operand.h>
#include <algorithm>
#include <iostream>

namespace ME
{
    void Mem2RegPass::runOnModule(Module& module)
    {
        for (auto* function : module.functions) runOnFunction(*function);
    }

    void Mem2RegPass::runOnFunction(Function& function)
    {
        // 1. Prerequisite: CFG and DomInfo
        Analysis::AM.get<Analysis::CFG>(function);
        domInfo = Analysis::AM.get<Analysis::DomInfo>(function);

        promotableAllocas.clear();
        defBlocks.clear();
        newPhis.clear();
        valueStacks.clear();
        replacements.clear();
        phiToAlloca.clear();

        // 2. Find allocas that can be promoted
        findPromotableAllocas(function);
        if (promotableAllocas.empty()) return;

        // 3. Insert Phi nodes
        insertPhiNodes(function);

        // 4. Rename variables
        std::set<size_t> visited;
        renameRecursive(0, function, visited);

        // 5. Remove allocas
        for (auto& [id, block] : function.blocks)
        {
            auto it = block->insts.begin();
            while (it != block->insts.end())
            {
                bool removed = false;
                if ((*it)->opcode == Operator::ALLOCA)
                {
                    AllocaInst* allocaInst = static_cast<AllocaInst*>(*it);
                    if (std::find(promotableAllocas.begin(), promotableAllocas.end(), allocaInst) != promotableAllocas.end())
                    {
                        delete *it;
                        it = block->insts.erase(it);
                        removed = true;
                    }
                }
                
                if (!removed) ++it;
            }
        }
    }

    void Mem2RegPass::findPromotableAllocas(Function& function)
    {
        std::map<size_t, std::vector<Instruction*>> useMap;
        std::vector<AllocaInst*> allAllocas;
        std::map<Instruction*, size_t> instToBlockId;

        for (auto& [id, block] : function.blocks)
        {
            for (auto* inst : block->insts)
            {
                instToBlockId[inst] = id;
                
                if (inst->opcode == Operator::ALLOCA)
                {
                    allAllocas.push_back(static_cast<AllocaInst*>(inst));
                }

                auto checkOp = [&](Operand* op) {
                    if (op && op->getType() == OperandType::REG)
                    {
                        useMap[op->getRegNum()].push_back(inst);
                    }
                };

                switch (inst->opcode) {
                    case Operator::LOAD: checkOp(static_cast<LoadInst*>(inst)->ptr); break;
                    case Operator::STORE: 
                        checkOp(static_cast<StoreInst*>(inst)->val);
                        checkOp(static_cast<StoreInst*>(inst)->ptr);
                        break;
                    case Operator::ADD: case Operator::SUB: case Operator::MUL: case Operator::DIV:
                    case Operator::MOD: case Operator::FADD:
                    case Operator::FSUB: case Operator::FMUL: case Operator::FDIV:
                    case Operator::BITAND: case Operator::BITXOR: case Operator::SHL:
                    case Operator::LSHR: case Operator::ASHR:
                        checkOp(static_cast<ArithmeticInst*>(inst)->lhs);
                        checkOp(static_cast<ArithmeticInst*>(inst)->rhs);
                        break;
                    case Operator::ICMP:
                        checkOp(static_cast<IcmpInst*>(inst)->lhs);
                        checkOp(static_cast<IcmpInst*>(inst)->rhs);
                        break;
                    case Operator::FCMP:
                        checkOp(static_cast<FcmpInst*>(inst)->lhs);
                        checkOp(static_cast<FcmpInst*>(inst)->rhs);
                        break;
                    case Operator::BR_COND: checkOp(static_cast<BrCondInst*>(inst)->cond); break;
                    case Operator::CALL:
                        for (auto& arg : static_cast<CallInst*>(inst)->args) checkOp(arg.second);
                        break;
                    case Operator::RET: checkOp(static_cast<RetInst*>(inst)->res); break;
                    case Operator::GETELEMENTPTR:
                        checkOp(static_cast<GEPInst*>(inst)->basePtr);
                        for (auto* idx : static_cast<GEPInst*>(inst)->idxs) checkOp(idx);
                        break;
                    case Operator::SITOFP: checkOp(static_cast<SI2FPInst*>(inst)->src); break;
                    case Operator::FPTOSI: checkOp(static_cast<FP2SIInst*>(inst)->src); break;
                    case Operator::ZEXT: checkOp(static_cast<ZextInst*>(inst)->src); break;
                    case Operator::PHI:
                        for (auto& [lbl, val] : static_cast<PhiInst*>(inst)->incomingVals) checkOp(val);
                        break;
                    default: break;
                }
            }
        }

        for (auto* allocaInst : allAllocas)
        {
            if (isPromotable(allocaInst, useMap))
            {
                promotableAllocas.push_back(allocaInst);
                
                size_t regNum = allocaInst->res->getRegNum();
                if (useMap.count(regNum)) {
                    for (auto* user : useMap.at(regNum)) {
                        if (user->opcode == Operator::STORE) {
                            defBlocks[allocaInst].insert(instToBlockId[user]);
                        }
                    }
                }
            }
        }
    }

    bool Mem2RegPass::isPromotable(AllocaInst* allocaInst, const std::map<size_t, std::vector<Instruction*>>& useMap)
    {
        if (!allocaInst->dims.empty()) return false;

        size_t regNum = allocaInst->res->getRegNum();
        if (useMap.find(regNum) == useMap.end()) return true;

        for (auto* user : useMap.at(regNum))
        {
            if (user->opcode == Operator::LOAD)
            {
                if (static_cast<LoadInst*>(user)->ptr->getRegNum() != regNum) return false;
            }
            else if (user->opcode == Operator::STORE)
            {
                if (static_cast<StoreInst*>(user)->ptr->getRegNum() != regNum) return false;
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    void Mem2RegPass::insertPhiNodes(Function& function)
    {
        const auto& domFrontier = domInfo->getDomFrontier();
        
        for (auto* allocaInst : promotableAllocas)
        {
            std::set<size_t> F;
            std::vector<size_t> W(defBlocks[allocaInst].begin(), defBlocks[allocaInst].end());
            
            size_t idx = 0;
            while (idx < W.size())
            {
                size_t X = W[idx++];
                if (X >= domFrontier.size()) continue;

                for (size_t Y : domFrontier[X])
                {
                    if (F.find(Y) == F.end())
                    {
                        Block* blockY = function.blocks[Y];
                        Operand* phiRes = getRegOperand(function.getNewRegId());
                        PhiInst* phi = new PhiInst(allocaInst->dt, phiRes);
                        
                        blockY->insertFront(phi);
                        newPhis[allocaInst].push_back(phi);
                        phiToAlloca[phi] = allocaInst;
                        
                        F.insert(Y);
                        if (defBlocks[allocaInst].find(Y) == defBlocks[allocaInst].end())
                        {
                            W.push_back(Y);
                        }
                    }
                }
            }
        }
    }
    void Mem2RegPass::renameRecursive(size_t blockId, Function& function, std::set<size_t>& visited)
    {
        if (visited.count(blockId)) return;
        visited.insert(blockId);
        
        Block* block = function.blocks[blockId];
        std::vector<AllocaInst*> pushedAllocas;
            
        // 1. Process Phis
        for (auto* inst : block->insts)
        {
            if (inst->opcode == Operator::PHI)
            {
                PhiInst* phi = static_cast<PhiInst*>(inst);
                if (phiToAlloca.count(phi))
                {
                    AllocaInst* alloca = phiToAlloca[phi];
                    valueStacks[alloca].push(phi->res);
                    pushedAllocas.push_back(alloca);
                }
            }
        }
        
        // 2. Process Instructions
        std::vector<Instruction*> toRemove;
        for (auto* inst : block->insts)
        {
            updateInstructionUses(inst);
            
            if (inst->opcode == Operator::LOAD)
            {
                LoadInst* load = static_cast<LoadInst*>(inst);
                AllocaInst* alloca = getAlloca(load->ptr);
                if (alloca && std::find(promotableAllocas.begin(), promotableAllocas.end(), alloca) != promotableAllocas.end())
                {
                    if (!valueStacks[alloca].empty())
                    {
                        replacements[load->res->getRegNum()] = valueStacks[alloca].top();
                        toRemove.push_back(inst);
                    }
                }
            }
            else if (inst->opcode == Operator::STORE)
            {
                StoreInst* store = static_cast<StoreInst*>(inst);
                AllocaInst* alloca = getAlloca(store->ptr);
                if (alloca && std::find(promotableAllocas.begin(), promotableAllocas.end(), alloca) != promotableAllocas.end())
                {
                    valueStacks[alloca].push(store->val);
                    pushedAllocas.push_back(alloca);
                    toRemove.push_back(inst);
                }
            }
        }
        
        for (auto* inst : toRemove)
        {
            auto it = std::find(block->insts.begin(), block->insts.end(), inst);
            if (it != block->insts.end())
            {
                block->insts.erase(it);
                delete inst;
            }
        }
        
        // 3. Fill Successor Phis
        Instruction* term = block->insts.empty() ? nullptr : block->insts.back();
        std::vector<size_t> successors;
        if (term)
        {
            if (term->opcode == Operator::BR_UNCOND)
            {
                BrUncondInst* br = static_cast<BrUncondInst*>(term);
                successors.push_back(static_cast<LabelOperand*>(br->target)->lnum);
            }
            else if (term->opcode == Operator::BR_COND)
            {
                BrCondInst* br = static_cast<BrCondInst*>(term);
                successors.push_back(static_cast<LabelOperand*>(br->trueTar)->lnum);
                successors.push_back(static_cast<LabelOperand*>(br->falseTar)->lnum);
            }
        }
        
        for (size_t succId : successors)
        {
            Block* succBlock = function.blocks[succId];
            for (auto* inst : succBlock->insts)
            {
                if (inst->opcode == Operator::PHI)
                {
                    PhiInst* phi = static_cast<PhiInst*>(inst);
                    if (phiToAlloca.count(phi))
                    {
                        AllocaInst* alloca = phiToAlloca[phi];
                        if (!valueStacks[alloca].empty())
                        {
                            phi->addIncoming(valueStacks[alloca].top(), getLabelOperand(blockId));
                        }
                        else
                        {
                            if (alloca->dt == DataType::I32)
                                phi->addIncoming(getImmeI32Operand(0), getLabelOperand(blockId));
                            else
                                phi->addIncoming(getImmeF32Operand(0.0), getLabelOperand(blockId));
                        }
                    }
                }
            }
        }
        
        // 4. Recurse
        const auto& domTree = domInfo->getDomTree();
        if (blockId < domTree.size())
        {
            for (int childId : domTree[blockId])
            {
                renameRecursive(childId, function, visited);
            }
        }
        
        // 5. Pop Stacks
        while (!pushedAllocas.empty())
        {
            AllocaInst* alloca = pushedAllocas.back();
            pushedAllocas.pop_back();
            valueStacks[alloca].pop();
        }
    }

    AllocaInst* Mem2RegPass::getAlloca(Operand* ptr)
    {
        if (ptr->getType() == OperandType::REG)
        {
            size_t reg = ptr->getRegNum();
            for (auto* alloca : promotableAllocas)
            {
                if (alloca->res->getRegNum() == reg) return alloca;
            }
        }
        return nullptr;
    }

    void Mem2RegPass::updateInstructionUses(Instruction* inst)
    {
        auto replace = [&](Operand*& op) {
            if (op && op->getType() == OperandType::REG) {
                size_t reg = op->getRegNum();
                if (replacements.count(reg)) {
                    op = replacements[reg];
                }
            }
        };

        switch (inst->opcode) {
            case Operator::ADD: case Operator::SUB: case Operator::MUL: case Operator::DIV:
            case Operator::MOD: case Operator::FADD:
            case Operator::FSUB: case Operator::FMUL: case Operator::FDIV:
            case Operator::BITAND: case Operator::BITXOR: case Operator::SHL:
            case Operator::LSHR: case Operator::ASHR:
            {
                auto* i = static_cast<ArithmeticInst*>(inst);
                replace(i->lhs);
                replace(i->rhs);
                break;
            }
            case Operator::LOAD:
            {
                auto* i = static_cast<LoadInst*>(inst);
                replace(i->ptr);
                break;
            }
            case Operator::STORE:
            {
                auto* i = static_cast<StoreInst*>(inst);
                replace(i->val);
                replace(i->ptr);
                break;
            }
            case Operator::ICMP:
            {
                auto* i = static_cast<IcmpInst*>(inst);
                replace(i->lhs);
                replace(i->rhs);
                break;
            }
            case Operator::FCMP:
            {
                auto* i = static_cast<FcmpInst*>(inst);
                replace(i->lhs);
                replace(i->rhs);
                break;
            }
            case Operator::CALL:
            {
                auto* i = static_cast<CallInst*>(inst);
                for (auto& arg : i->args) {
                    replace(arg.second);
                }
                break;
            }
            case Operator::BR_COND:
            {
                auto* i = static_cast<BrCondInst*>(inst);
                replace(i->cond);
                break;
            }
            case Operator::RET:
            {
                auto* i = static_cast<RetInst*>(inst);
                if (i->res) replace(i->res);
                break;
            }
            case Operator::GETELEMENTPTR:
            {
                auto* i = static_cast<GEPInst*>(inst);
                replace(i->basePtr);
                for (auto* idx : i->idxs) replace(idx);
                break;
            }
            case Operator::SITOFP:
            {
                auto* i = static_cast<SI2FPInst*>(inst);
                replace(i->src);
                break;
            }
            case Operator::FPTOSI:
            {
                auto* i = static_cast<FP2SIInst*>(inst);
                replace(i->src);
                break;
            }
            case Operator::ZEXT:
            {
                auto* i = static_cast<ZextInst*>(inst);
                replace(i->src);
                break;
            }
            case Operator::PHI:
            {
                auto* i = static_cast<PhiInst*>(inst);
                for (auto& [lbl, val] : i->incomingVals) {
                    replace(val);
                }
                break;
            }
            default: break;
        }
    }
}
