#ifndef __MACHINE_STRAIGHTEN_H__
#define __MACHINE_STRAIGHTEN_H__

#include "MachineCode.h"

class MachineStraighten {
    MachineUnit* unit;

   public:
    MachineStraighten(MachineUnit* unit) : unit(unit){};
    void pass();
    void pass1(MachineFunction* func);
    void pass2(MachineFunction* func);
    void pass3(MachineFunction* func);
    void pass4(MachineFunction* func);
    void fuseBlock(MachineFunction* func, MachineBlock* i, MachineBlock* j);
};

#endif