#ifndef __CLEAN_ASM_ADD_ZERO_H__
#define __CLEAN_ASM_ADD_ZERO_H__

#include "MachineCode.h"

class CleanAsmAddZero {
    MachineUnit* unit;

   public:
    CleanAsmAddZero(MachineUnit* unit) : unit(unit){};
    void pass();
    void pass(MachineFunction* func);
};

#endif