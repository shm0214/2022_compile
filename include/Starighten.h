#ifndef __STARIGHTEN_H__
#define __STARIGHTEN_H__

#include "Unit.h"

class Starighten {
    Unit* unit;

   public:
    Starighten(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* function);
    void fuseBlock(Function* function, BasicBlock* i, BasicBlock* j);
};

#endif