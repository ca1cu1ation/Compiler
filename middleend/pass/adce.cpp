#include <middleend/pass/adce.h>
#include <middleend/module/ir_module.h>
#include <middleend/pass/analysis/analysis_manager.h>
#include <middleend/pass/analysis/cfg.h>
#include <middleend/pass/analysis/dominfo.h>
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
        // 确保已构建CFG并移除不可达基本块
        auto* cfg = Analysis::AM.get<Analysis::CFG>(function);

        // 计算后支配边界
        std::map<Block*, std::set<Block*>> pdf;
        computePostDominance(function, pdf);

        liveInsts.clear();
        worklist.clear();
        defMap.clear();
        instToBlock.clear();

        buildDefMap(function);

        // 用关键指令初始化工作队列
        // 关键指令：有副作用（Store、Call、Ret），分支条件（如 BR_COND 的条件操作数）以及终结指令（用于保持CFG结构）
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
                    case Operator::BR_COND: {
                        // 分支条件相关
                        auto* cond = static_cast<BrCondInst*>(inst)->cond;
                        Instruction* condDef = getDef(cond);
                        // 检查分支目标块是否在 pdf 中
                        for (auto* succ : cfg->getSuccessors(block)) {
                            if (pdf[block].count(succ)) {
                                // 条件影响控制流
                                if (condDef) markLive(condDef);
                            }
                        }
                        break;
                    }
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

        // 传播活跃性
        while (!worklist.empty())
        {
            Instruction* inst = worklist.back();
            worklist.pop_back();

            // 数据依赖
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

        // 移除无用指令
        for (auto& [id, block] : function.blocks)
        {
            auto it = block->insts.begin();
            while (it != block->insts.end())
            {
                Instruction* inst = *it;
                if (!isLive(inst))
                {
                    // 再次确认不是终结指令（终结指令应为活跃）
                    if (inst->isTerminator())
                    {
                        // 如果所有终结指令都已标记为活跃则不会发生
                        ++it;
                    }
                    else
                    {
                        // 移除指令
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
        // 标记指令为活跃并加入工作队列
    }

    bool ADCEPass::isLive(Instruction* inst)
    {
        return liveInsts.find(inst) != liveInsts.end(); // 判断指令是否活跃
    }

    void ADCEPass::buildDefMap(Function& function)
    {
        for (auto& [id, block] : function.blocks)
        {
            for (auto* inst : block->insts)
            {
                instToBlock[inst] = block;
                // 记录指令所属基本块
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
                // 构建寄存器号到定义指令的映射
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
        // 获取操作数的定义指令
        return nullptr;
    }
    
    std::vector<Operand*> ADCEPass::getUses(Instruction* inst)
    {
        std::vector<Operand*> uses; // 存储指令使用的操作数
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
        // 1. 构建CFG和后支配树
        auto* cfg = Analysis::AM.get<Analysis::CFG>(function);
        auto* domInfo = Analysis::AM.get<Analysis::DomInfo>(function);
        
        // 2. 获取每个块的直接后支配者
        const auto& immPostDom = domInfo->getImmDom();

        // 3. 计算PDF（后支配边界）
        // 遍历所有块
        for (auto& [id, block] : function.blocks) {
            // 获取前驱
            for (size_t predId : cfg->invG_id[id]) {
                Block* runner = cfg->id2block[predId];
                while (runner && runner != cfg->id2block[immPostDom[id]]) {
                    pdf[runner].insert(block);
                    size_t runnerId = runner->blockId;
                    if (immPostDom[runnerId] < 0 || !cfg->id2block.count(immPostDom[runnerId])) break;
                    runner = cfg->id2block[immPostDom[runnerId]];
                }
            }
        }
    }
}
