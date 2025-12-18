#ifndef __MIDDLEEND_PASS_MEM2REG_H__
#define __MIDDLEEND_PASS_MEM2REG_H__

#include <interfaces/middleend/pass.h>
#include <middleend/module/ir_function.h>
#include <middleend/module/ir_instruction.h>
#include <middleend/pass/analysis/dominfo.h>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <unordered_set>

namespace ME
{
    class Mem2RegPass : public FunctionPass
    {
    public:
        Mem2RegPass() = default;
        ~Mem2RegPass() = default;

        void runOnFunction(Function& function) override;
        void runOnModule(Module& module) override;

    private:
        Analysis::DomInfo* domInfo; // 支配信息
        std::map<PhiInst*, AllocaInst*> phiToAlloca; // Phi节点到Alloca的映射
        
        // 可提升的alloca集合
        std::vector<AllocaInst*> promotableAllocas;
        std::unordered_set<AllocaInst*> promotableAllocasSet;
        
        // 每个alloca在被store的块编号集合
        std::map<AllocaInst*, std::set<size_t>> defBlocks;
        
        // 每个alloca插入的phi节点列表
        std::map<AllocaInst*, std::vector<PhiInst*>> newPhis;

        // 变量重命名用的栈：Alloca -> 当前值栈
        std::map<AllocaInst*, std::stack<Operand*>> valueStacks;

        // 替换表：RegNum -> Operand
        std::vector<Operand*> replacements;
        
        // RegNum到Alloca的映射
        std::vector<AllocaInst*> regToAlloca;

        // 查找可提升的alloca
        void findPromotableAllocas(Function& function); 
        // 判断alloca是否可提升
        bool isPromotable(AllocaInst* allocaInst, const std::vector<std::vector<std::pair<Instruction*, size_t>>>& useMap);
        // 为可提升alloca插入phi节点
        void insertPhiNodes(Function& function);
        // 递归重命名变量
        void renameVariables(Function& function);
        // 重命名递归辅助函数
        void renameRecursive(size_t blockId, Function& function, std::set<size_t>& visited);
        // 替换指令中的寄存器引用
        void updateInstructionUses(Instruction* inst);
        // 根据指针操作数获取对应的alloca
        AllocaInst* getAlloca(Operand* ptr);
    };
}

#endif
