#ifndef __MACHINE_DEAD_CODE_ELIMINATION_H__
#define __MACHINE_DEAD_CODE_ELIMINATION_H__

#include "MachineCode.h"

class MachineDeadCodeElimination {
    MachineUnit* unit;

   public:
    MachineDeadCodeElimination(MachineUnit* unit) : unit(unit){};
    void pass();
    void pass(MachineFunction* func);
};

#endif