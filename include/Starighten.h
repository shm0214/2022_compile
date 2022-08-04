#ifndef __STARIGHTEN_H__
#define __STARIGHTEN_H__

#include "Unit.h"

class Starighten {
    Unit* unit;
    std::map<BasicBlock*, std::vector<BasicBlock*>> changes{};

   public:
    Starighten(Unit* unit) : unit(unit){};
    void pass();
    void pass1(Function* function);
    void pass2(Function* function);
    void pass3(Function* function);
    void pass4(Function* function);
    void fuseBlock(Function* function, BasicBlock* i, BasicBlock* j);
    void checkPhi(Function* function);
    void checkAllocAndPhi(Function* function);
};

#endif