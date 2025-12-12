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
        Analysis::AM.get<Analysis::CFG>(function);

        // 计算后支配边界
        std::map<Block*, std::set<Block*>> pdf;
        computePostDominance(function, pdf);

        liveInsts.clear();
        worklist.clear();
        defMap.clear();
        instToBlock.clear();

        buildDefMap(function);

        // 用关键指令初始化工作队列
        // 关键指令：有副作用（Store、Call、Ret）
        // 注意：分支指令初始时不标记为活跃，除非它们控制了活跃指令
        for (auto& [id, block] : function.blocks)
        {
            for (auto* inst : block->insts)
            {
                bool isCritical = false;
                switch (inst->opcode)
                {
                case Operator::STORE:
                case Operator::CALL:
                case Operator::RET:
                    isCritical = true;
                    break;
                default:
                    break;
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

            // 1. 数据依赖：标记操作数定义指令
            std::vector<Operand*> uses = getUses(inst);
            for (auto* op : uses)
            {
                Instruction* defInst = getDef(op);
                if (defInst && !isLive(defInst))
                {
                    markLive(defInst);
                }
            }

            // 2. 控制依赖：标记控制该指令所在块的终结指令
            // 如果指令 inst 活跃，那么它所在的块 block 必须执行
            // 因此，控制 block 执行的块（即 PDF 中的块）的终结指令必须活跃
            Block* block = instToBlock[inst];
            if (block) {
                for (Block* cdBlock : pdf[block]) {
                    Instruction* terminator = cdBlock->insts.back();
                    if (terminator && terminator->isTerminator() && !isLive(terminator)) {
                        markLive(terminator);
                    }
                }
            }
            
            // 特殊处理 Phi 节点：Phi 的活跃性意味着我们需要控制流到达 Phi 所在的块
            // 并且 Phi 的操作数（来自前驱的值）是活跃的。
            // 上面的 PDF 逻辑处理了“到达 Phi 所在块”的控制依赖。
            // 数据依赖循环处理了操作数的活跃性。
            // 还需要确保前驱块到当前块的边存在？
            // 如果前驱块的终结指令活跃，边就存在。
            // Phi 的操作数是 (Value, Label)。Label 对应前驱块。
            // 如果 Phi 活跃，是否意味着 Label 对应的前驱块的终结指令必须活跃？
            // 是的，因为我们需要从那个前驱跳转过来才能得到那个值。
            // 但是，Phi 节点本身的选择逻辑是在运行时根据来源决定的。
            // 如果我们标记了 Value 的定义活跃，Value 定义所在的块也就活跃了。
            // 如果 Value 是常量，没有定义指令。
            // 无论如何，标准 ADCE 中，Phi 的活跃性通过操作数的数据依赖和所在块的控制依赖来传播。
            // 额外的：对于 Phi 指令，我们需要标记它所有 incoming edges 对应的 block 的 terminator 为 live
            // 这样才能保证控制流能流转到这里，并且携带正确的值
            if (inst->opcode == Operator::PHI) {
                PhiInst* phi = static_cast<PhiInst*>(inst);
                for (auto& [label, val] : phi->incomingVals) {
                    if (label->getType() == OperandType::LABEL) {
                        size_t predId = static_cast<LabelOperand*>(label)->lnum;
                        if (function.blocks.count(predId)) {
                            Block* predBlock = function.blocks[predId];
                            if (!predBlock->insts.empty()) {
                                Instruction* term = predBlock->insts.back();
                                if (term->isTerminator() && !isLive(term)) {
                                    markLive(term);
                                }
                            }
                        }
                    }
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
                    if (inst->isTerminator())
                    {
                        // 如果终结指令不活跃（例如死循环的分支，或者无用的分支）
                        // 我们需要将其替换为跳转到后支配者的无条件跳转
                        // 这样可以切断死循环或跳过无用代码
                        if (inst->opcode == Operator::BR_COND) {
                            BrCondInst* br = static_cast<BrCondInst*>(inst);
                            size_t trueL = static_cast<LabelOperand*>(br->trueTar)->lnum;
                            size_t falseL = static_cast<LabelOperand*>(br->falseTar)->lnum;
                            
                            int ipdom = (id < immPostDom.size()) ? immPostDom[id] : -1;
                            
                            Operand* target = nullptr;
                            
                            // 优先跳转到后支配者（退出循环/跳过分支）
                            if (ipdom != -1) {
                                if (trueL == static_cast<size_t>(ipdom)) target = br->trueTar;
                                else if (falseL == static_cast<size_t>(ipdom)) target = br->falseTar;
                            }
                            
                            // 如果都不是直接后支配者，任选一个（通常选 true）
                            // 注意：在死循环中，如果 true 指向循环体，false 指向退出（即 ipdom），上面的逻辑会选中 false
                            // 如果两个都不是 ipdom（例如嵌套结构），任选一个通常是安全的，因为它们最终都会汇聚
                            if (!target) target = br->trueTar;
                            
                            // 替换为 BR_UNCOND
                            auto* newBr = new BrUncondInst(target);
                            *it = newBr;
                            delete inst;
                            
                            // 注意：这里改变了 CFG 结构，但 ADCE 通常不负责清理不可达块
                            // 不可达块将在后续的 CFG 清理或下一次 ADCE 中被移除
                        } else if (inst->opcode == Operator::BR_UNCOND) {
                            // BR_UNCOND 通常应该被标记为活跃（如果它所在的块活跃）
                            // 如果它不活跃，说明它所在的块本身就是死代码的一部分？
                            // 或者它是冗余的跳转？
                            // 无论如何，保留它通常是安全的，或者可以删除它如果它是 fallthrough（但 IR 中没有 fallthrough）
                            // 这里我们选择保留，以免破坏 CFG
                        }
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
        auto* cfg = Analysis::AM.get<Analysis::CFG>(function);
        
        // 1. 准备 DomAnalyzer 需要的输入
        // 反向图：DomAnalyzer(reverse=true) 会自动反转图，所以我们传入正向图 G
        // 入口点：反向图的入口点是原图的出口点（RET 块）
        std::vector<std::vector<int>> graph(cfg->G_id.size());
        for(size_t i=0; i<cfg->G_id.size(); ++i) {
            for(auto succ : cfg->G_id[i]) graph[i].push_back((int)succ);
        }

        std::vector<int> exits;
        for (auto& [id, block] : function.blocks) {
            if (!block->insts.empty() && block->insts.back()->opcode == Operator::RET) {
                exits.push_back((int)id);
            }
        }

        // 2. 计算后支配树
        DomAnalyzer postDomAnalyzer;
        postDomAnalyzer.solve(graph, exits, true);
        
        immPostDom = postDomAnalyzer.imm_dom;

        // 3. 映射 PDF
        // DomAnalyzer 的 dom_frontier 即为 PDF
        for(size_t i=0; i<postDomAnalyzer.dom_frontier.size(); ++i) {
            if (!cfg->id2block.count(i)) continue;
            Block* B = cfg->id2block[i];
            
            for(int pdf_idx : postDomAnalyzer.dom_frontier[i]) {
                if(cfg->id2block.count(pdf_idx)) {
                    pdf[B].insert(cfg->id2block[pdf_idx]);
                }
            }
        }
    }
}
