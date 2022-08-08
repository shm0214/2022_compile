#ifndef __MACHINE_STRAIGHTEN_H__
#define __MACHINE_STRAIGHTEN_H__

#include "MachineCode.h"

class MachineStraighten {
    MachineUnit* unit;

   public:
    MachineStraighten(MachineUnit* unit) : unit(unit){};
    void pass();
    bool pass1(MachineFunction* func);
    bool pass2(MachineFunction* func);
    bool pass3(MachineFunction* func);
    bool pass4(MachineFunction* func);
    void fuseBlock(MachineFunction* func, MachineBlock* i, MachineBlock* j);
};

#endif