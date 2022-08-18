#ifndef __LOCAL_VALUE_NUMBER_H__
#define __LOCAL_VALUE_NUMBER_H__

#include "MachineCode.h"

class LocalValueNumber {
    MachineUnit* unit;

   public:
    LocalValueNumber(MachineUnit* unit) : unit(unit){};
    void pass();
    void pass(MachineFunction* func);
    void pass(MachineBlock* block);
    void changeMul2Exp2LSL();
};

#endif