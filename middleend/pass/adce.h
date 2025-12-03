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
        std::set<Instruction*> liveInsts; // Set of live instructions
        std::vector<Instruction*> worklist; // Worklist for liveness propagation
        std::map<size_t, Instruction*> defMap; // RegNum -> Defining Instruction
        std::map<Instruction*, Block*> instToBlock; // Instruction -> Parent Block

        void markLive(Instruction* inst); // Mark instruction as live
        bool isLive(Instruction* inst); // Check if instruction is live
        
        void buildDefMap(Function& function); // Build definition map
        std::vector<Operand*> getUses(Instruction* inst); // Get uses of an instruction
        Instruction* getDef(Operand* op); // Get defining instruction of an operand
        
        // Control Dependence
        void computePostDominance(Function& function, std::map<Block*, std::set<Block*>>& pdf);
    };
}

#endif
