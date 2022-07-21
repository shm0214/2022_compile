#ifndef __SSADESTRUCTION_H__
#define __SSADESTRUCTION_H__

#include "Unit.h"

class SSADestruction {
    Unit* unit;

   public:
    SSADestruction(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* function);
};

#endif