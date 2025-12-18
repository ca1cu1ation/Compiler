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
        // 1. 前置条件：获取CFG和支配信息
        // 先失效分析结果，确保CFG和支配树是最新的
        Analysis::AM.invalidate(function); // 强制失效，保证CFG/DomInfo新鲜
        Analysis::AM.get<Analysis::CFG>(function);
        domInfo = Analysis::AM.get<Analysis::DomInfo>(function);

        // 初始化各类成员变量
        promotableAllocas.clear();
        defBlocks.clear();
        newPhis.clear();
        valueStacks.clear();
        replacements.assign(function.getMaxReg() + 1, nullptr);
        regToAlloca.assign(function.getMaxReg() + 1, nullptr);
        phiToAlloca.clear();

        // 2. 查找可以提升为SSA寄存器的alloca指令
        findPromotableAllocas(function);
        if (promotableAllocas.empty()) return; // 没有可提升的alloca则直接返回

        // 3. 插入Phi节点
        insertPhiNodes(function);

        // 4. 变量重命名，递归遍历支配树
        std::set<size_t> visited;
        renameRecursive(0, function, visited);

        // 5. 移除已提升的alloca指令
        for (auto& [id, block] : function.blocks)
        {
            auto it = block->insts.begin();
            while (it != block->insts.end())
            {
                bool removed = false;
                if ((*it)->opcode == Operator::ALLOCA)
                {
                    AllocaInst* allocaInst = static_cast<AllocaInst*>(*it);
                    if (promotableAllocasSet.count(allocaInst))
                    {
                        // 删除已提升的alloca指令
                        delete *it;
                        it = block->insts.erase(it);
                        removed = true;
                    }
                }
                
                if (!removed) ++it;
            }
        }
    }


    // 查找可以提升为SSA寄存器的alloca指令
    // 原理：只要alloca只被load/store访问，且没有数组维度，就可以提升
    void Mem2RegPass::findPromotableAllocas(Function& function)
    {
        // useMap记录每个寄存器的所有使用（指令和所在块）
        std::vector<std::vector<std::pair<Instruction*, size_t>>> useMap(function.getMaxReg() + 1);
        std::vector<AllocaInst*> allAllocas;
        promotableAllocasSet.clear();

        // 遍历所有指令，收集alloca和寄存器的使用情况
        for (auto& [id, block] : function.blocks)
        {
            for (auto* inst : block->insts)
            {
                if (inst->opcode == Operator::ALLOCA)
                {
                    allAllocas.push_back(static_cast<AllocaInst*>(inst));
                }

                // 检查操作数是否为寄存器，记录其使用
                auto checkOp = [&](Operand* op) {
                    if (op && op->getType() == OperandType::REG)
                    {
                        size_t reg = op->getRegNum();
                        if (reg < useMap.size()) {
                            useMap[reg].push_back({inst, id});
                        }
                    }
                };

                // 针对不同指令类型，检查其相关操作数
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

        // 检查每个alloca是否可提升
        for (auto* allocaInst : allAllocas)
        {
            if (isPromotable(allocaInst, useMap))
            {
                promotableAllocas.push_back(allocaInst);
                promotableAllocasSet.insert(allocaInst);
                
                // 记录reg号到alloca的映射
                size_t regNum = allocaInst->res->getRegNum();
                if (regNum < regToAlloca.size()) {
                    regToAlloca[regNum] = allocaInst;
                }

                // 记录所有store所在的块，后续插入phi用
                if (regNum < useMap.size()) {
                    for (auto& [user, blockId] : useMap[regNum]) {
                        if (user->opcode == Operator::STORE) {
                            defBlocks[allocaInst].insert(blockId);
                        }
                    }
                }
            }
        }
    }


    // 判断一个alloca是否可以提升为SSA寄存器
    // 原理：只能被load/store访问，且没有数组维度
    bool Mem2RegPass::isPromotable(AllocaInst* allocaInst, const std::vector<std::vector<std::pair<Instruction*, size_t>>>& useMap)
    {
        if (!allocaInst->dims.empty()) return false; // 有数组维度不能提升

        size_t regNum = allocaInst->res->getRegNum();
        if (regNum >= useMap.size()) return true;

        // 检查所有使用，必须全是load/store且指向自己
        for (auto& [user, blockId] : useMap[regNum])
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


    // 为每个可提升的alloca插入Phi节点
    // 原理：在支配边界（DF）上插入Phi，保证SSA形式
    void Mem2RegPass::insertPhiNodes(Function& function)
    {
        const auto& domFrontier = domInfo->getDomFrontier();
        
        for (auto* allocaInst : promotableAllocas)
        {
            std::set<size_t> F; // 已插入Phi的块
            std::vector<size_t> W(defBlocks[allocaInst].begin(), defBlocks[allocaInst].end()); // 定义块队列
            
            size_t idx = 0;
            while (idx < W.size())
            {
                size_t X = W[idx++];
                if (X >= domFrontier.size()) continue;

                for (size_t Y : domFrontier[X])
                {
                    if (F.find(Y) == F.end())
                    {
                        // 在Y块插入Phi节点
                        Block* blockY = function.blocks[Y];
                        Operand* phiRes = getRegOperand(function.getNewRegId());
                        PhiInst* phi = new PhiInst(allocaInst->dt, phiRes);
                        
                        blockY->insertFront(phi);
                        newPhis[allocaInst].push_back(phi);
                        phiToAlloca[phi] = allocaInst;
                        
                        F.insert(Y);
                        // 若Y不是定义块，继续向外扩展
                        if (defBlocks[allocaInst].find(Y) == defBlocks[allocaInst].end())
                        {
                            W.push_back(Y);
                        }
                    }
                }
            }
        }
    }

    // 递归重命名变量，完成SSA形式的构建
    // 原理：遍历支配树，维护每个alloca的值栈，遇到load/store/phi时更新
    void Mem2RegPass::renameRecursive(size_t blockId, Function& function, std::set<size_t>& visited)
    {
        if (visited.count(blockId)) return;
        visited.insert(blockId);
        
        Block* block = function.blocks[blockId];
        std::vector<AllocaInst*> pushedAllocas;
            
        // 1. 处理Phi节点：将新Phi结果压入对应alloca的值栈
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
        
        // 2. 处理普通指令：load用栈顶替换，store压栈，均可删除
        std::vector<Instruction*> toRemove;
        for (auto* inst : block->insts)
        {
            updateInstructionUses(inst); // 替换指令中的寄存器引用
            
            if (inst->opcode == Operator::LOAD)
            {
                LoadInst* load = static_cast<LoadInst*>(inst);
                AllocaInst* alloca = getAlloca(load->ptr);
                if (alloca && promotableAllocasSet.count(alloca))
                {
                    if (!valueStacks[alloca].empty())
                    {
                        // 用栈顶值替换load结果
                        replacements[load->res->getRegNum()] = valueStacks[alloca].top();
                        toRemove.push_back(inst);
                    }
                }
            }
            else if (inst->opcode == Operator::STORE)
            {
                StoreInst* store = static_cast<StoreInst*>(inst);
                AllocaInst* alloca = getAlloca(store->ptr);
                if (alloca && promotableAllocasSet.count(alloca))
                {
                    // store值压入栈顶
                    valueStacks[alloca].push(store->val);
                    pushedAllocas.push_back(alloca);
                    toRemove.push_back(inst);
                }
            }
        }
        
        // 删除已被替换的load/store指令
        for (auto* inst : toRemove)
        {
            auto it = std::find(block->insts.begin(), block->insts.end(), inst);
            if (it != block->insts.end())
            {
                block->insts.erase(it);
                delete inst;
            }
        }
        
        // 3. 填充后继块的Phi节点
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
                            // 用当前栈顶值作为来自本块的输入
                            phi->addIncoming(valueStacks[alloca].top(), getLabelOperand(blockId));
                        }
                        else
                        {
                            // 没有值则补默认值
                            if (alloca->dt == DataType::I32)
                                phi->addIncoming(getImmeI32Operand(0), getLabelOperand(blockId));
                            else
                                phi->addIncoming(getImmeF32Operand(0.0), getLabelOperand(blockId));
                        }
                    }
                }
            }
        }
        
        // 4. 递归处理支配树的子节点
        const auto& domTree = domInfo->getDomTree();
        if (blockId < domTree.size())
        {
            for (int childId : domTree[blockId])
            {
                renameRecursive(childId, function, visited);
            }
        }
        
        // 5. 回溯时弹出本块压入的所有alloca值
        while (!pushedAllocas.empty())
        {
            AllocaInst* alloca = pushedAllocas.back();
            pushedAllocas.pop_back();
            valueStacks[alloca].pop();
        }
    }


    // 根据指针操作数获取对应的alloca指令
    AllocaInst* Mem2RegPass::getAlloca(Operand* ptr)
    {
        if (ptr->getType() == OperandType::REG)
        {
            size_t reg = ptr->getRegNum();
            if (reg < regToAlloca.size()) {
                return regToAlloca[reg];
            }
        }
        return nullptr;
    }

    // 替换指令中的寄存器引用为最新SSA值
    void Mem2RegPass::updateInstructionUses(Instruction* inst)
    {
        auto replace = [&](Operand*& op) {
            if (op && op->getType() == OperandType::REG) {
                size_t reg = op->getRegNum();
                if (reg < replacements.size() && replacements[reg]) {
                    op = replacements[reg];
                }
            }
        };

        // 针对不同指令类型，替换其相关操作数
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
                for (auto& idx : i->idxs) replace(idx);
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
