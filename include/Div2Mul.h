#ifndef __DIV_TO_MUL_H
#define __DIV_TO_MUL_H

#include "MachineCode.h"
class Div2Mul {
    MachineUnit* unit;
   public:
    Div2Mul(MachineUnit* unit) : unit(unit){};
    void pass();
    void div2mul(MachineFunction *);
    ~Div2Mul();
};

#endif