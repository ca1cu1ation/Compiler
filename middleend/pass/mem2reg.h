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
        Analysis::DomInfo* domInfo;
        std::map<PhiInst*, AllocaInst*> phiToAlloca;
        
        // Promotable allocas
        std::vector<AllocaInst*> promotableAllocas;
        
        // For each alloca, list of blocks that define it (store to it)
        std::map<AllocaInst*, std::set<size_t>> defBlocks;
        
        // For each alloca, list of phi instructions inserted
        std::map<AllocaInst*, std::vector<PhiInst*>> newPhis;

        // Renaming stacks: Alloca -> Stack of current value
        std::map<AllocaInst*, std::stack<Operand*>> valueStacks;

        // Replacements: RegNum -> Operand
        std::map<size_t, Operand*> replacements;

        // Identify allocas that can be promoted
        void findPromotableAllocas(Function& function); 
        // Check if an alloca is promotable
        bool isPromotable(AllocaInst* allocaInst, const std::map<size_t, std::vector<Instruction*>>& useMap);
        // Insert phi nodes for promotable allocas
        void insertPhiNodes(Function& function);
        // Rename variables recursively
        void renameVariables(Function& function);
        // Recursive helper for renaming
        void renameRecursive(size_t blockId, Function& function, std::set<size_t>& visited);
        // Update instruction uses based on replacements
        void updateInstructionUses(Instruction* inst);
        // Get the alloca instruction corresponding to a pointer operand
        AllocaInst* getAlloca(Operand* ptr);
    };
}

#endif
