#ifndef __SPECIAL_OPTIMIZE_H__
#define __SPECIAL_OPTIMIZE_H__

#include "Unit.h"

class SpecialOptimize {
    Unit* unit;

   public:
    SpecialOptimize(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* function);
};

#endif