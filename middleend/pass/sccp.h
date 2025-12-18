#ifndef __MIDDLEEND_PASS_SCCP_H__
#define __MIDDLEEND_PASS_SCCP_H__

#include <interfaces/middleend/pass.h>
#include <middleend/module/ir_function.h>
#include <middleend/module/ir_instruction.h>
#include <middleend/module/ir_operand.h>
#include <map>
#include <set>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace ME
{
    class SCCPPass : public FunctionPass
    {
    public:
        SCCPPass() = default;
        ~SCCPPass() = default;

        void runOnFunction(Function& function) override;
        void runOnModule(Module& module) override;

    private:
        // 格类型：
        // TOP：未知
        // CONSTANT：已知常量
        // BOTTOM：不可常量传播（overdefined）
        enum LatticeType { TOP, CONSTANT, BOTTOM };
        
        // 格值结构体
        struct LatticeVal {
            LatticeType type;
            Operand* val; // 仅当type为CONSTANT时有效

            bool operator==(const LatticeVal& other) const;
            bool operator!=(const LatticeVal& other) const { return !(*this == other); }
        };

        // SCCP状态
        std::unordered_map<size_t, LatticeVal> latticeValues; // RegNum -> 格值
        std::unordered_set<size_t> executableBlocks; // 可达块编号集合
        std::set<std::pair<size_t, size_t>> executableEdges; // 可达边集合 {前驱, 后继}
        
        std::queue<std::pair<Block*, Block*>> flowWorklist; // 流敏感工作队列
        std::queue<Instruction*> ssaWorklist; // SSA工作队列

        std::unordered_map<size_t, std::vector<Instruction*>> useMap; // RegNum -> 使用者指令列表
        std::map<Instruction*, Block*> instBlockMap; // 指令到所属块的映射
        Function* currentFunction; // 当前处理的函数

        // 辅助函数
        void initialize(Function& function); // 初始化SCCP状态
        void buildUseMap(Function& function); // 构建useMap
        
        void markBlockExecutable(Block* block); // 标记块为可达
        void markEdgeExecutable(Block* pred, Block* succ); // 标记边为可达
        
        void visitPhi(PhiInst* phi); // 处理Phi指令
        void visitInstruction(Instruction* inst); // 处理非Phi指令
        
        LatticeVal evaluate(Instruction* inst); // 常量折叠指令，返回格值
        LatticeVal getValueState(Operand* op); // 获取操作数格值
        
        void markOverdefined(Operand* op); // 标记寄存器为overdefined
        void markConstant(Operand* op, Operand* constant); // 标记寄存器为常量
        
        bool replaceConstants(Function& function); // 替换常量传播结果
        
        // 常量折叠辅助
        bool areConstantsEqual(Operand* op1, Operand* op2);
        Operand* computeBinary(Operator op, Operand* lhs, Operand* rhs);
        Operand* computeIcmp(ICmpOp op, Operand* lhs, Operand* rhs);
        Operand* computeFcmp(FCmpOp op, Operand* lhs, Operand* rhs);
    };
}

#endif
