#ifndef __ELIM_UNREACH_CODE_H
#define __ELIM_UNREACH_CODE_H

#include <vector>
#include "Unit.h"

class ElimUnreachCode {
    Unit* unit;

   public:
    ElimUnreachCode(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* function);
    std::vector<int> getReachBlocks(Function* func, int idx);
};

#endif