#ifndef __PEEPHOLE_FOR_IR_H__
#define __PEEPHOLE_FOR_IR_H__

#include "Unit.h"

class PeepholeForIR {
    Unit* unit;

   public:
    PeepholeForIR(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* function);
};

#endif