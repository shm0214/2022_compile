#ifndef __SPECIAL_OPTIMIZE_H__
#define __SPECIAL_OPTIMIZE_H__

#include "Unit.h"

class SpecialOptimize {
    Unit* unit;

   public:
    SpecialOptimize(Unit* unit) : unit(unit){};
    void pass();
    void pass1();
    void pass(Function* function);
    void pass1(Function* function);
    void checkOperandsUse(Function* function);
};

#endif