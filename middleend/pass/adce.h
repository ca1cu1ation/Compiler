#ifndef __MIDDLEEND_PASS_ADCE_H__
#define __MIDDLEEND_PASS_ADCE_H__

#include <interfaces/middleend/pass.h>
#include <middleend/module/ir_function.h>
#include <middleend/module/ir_instruction.h>
#include <set>
#include <vector>
#include <map>

namespace ME
{
    class ADCEPass : public FunctionPass
    {
    public:
        ADCEPass() = default;
        ~ADCEPass() = default;

        void runOnFunction(Function& function) override;
        void runOnModule(Module& module) override;

    private:
        std::set<Instruction*> liveInsts; // 活跃指令集合
        std::vector<Instruction*> worklist; // 活跃性传播的工作队列
        std::map<size_t, Instruction*> defMap; // RegNum -> 定义该寄存器的指令
        std::map<Instruction*, Block*> instToBlock; // 指令 -> 所属基本块

        void markLive(Instruction* inst); // 标记指令为活跃
        bool isLive(Instruction* inst); // 检查指令是否活跃
        
        void buildDefMap(Function& function); // 构建定义映射表
        std::vector<Operand*> getUses(Instruction* inst); // 获取指令使用的操作数
        Instruction* getDef(Operand* op); // 获取操作数的定义指令
        
        // 计算后支配边界
        std::vector<int> immPostDom;
        void computePostDominance(Function& function, std::map<Block*, std::set<Block*>>& pdf);
    };
}

#endif
