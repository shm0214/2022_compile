#ifndef __INSTRUCTION_SCHEDULING__
#define __INSTRUCTION_SCHEDULING__

#include "MachineCode.h"

#include <list>
#include <vector>

class InstructionScheduling {
   public:
    InstructionScheduling(MachineUnit* mUnit);
    void pass();

   private:
    MachineUnit* mUnit;
    MachineBlock* block;

    std::vector<std::vector<int>> dep;
    std::vector<std::vector<int>> succs;
    std::vector<std::vector<int>> preds;
    std::vector<int> degree;  // counts of operands not ready yet

    std::vector<MachineInstruction*> insts;
    std::vector<int> priority;
    std::vector<int> ready;
    std::vector<int> active;

    std::vector<int> sched;
    std::vector<int> time;

    void compute_priority_helper(int x);

    void init();
    void build();
    void compute_priority();
    void schedule();
    void modify();
};

#endif  // __INSTRUCTION_SCHEDULING__