#ifndef __STARIGHTEN_H__
#define __STARIGHTEN_H__

#include "Unit.h"

class Starighten {
    Unit* unit;
    std::map<BasicBlock*, std::vector<BasicBlock*>> changes{};

   public:
    Starighten(Unit* unit) : unit(unit){};
    void pass();
    bool pass1(Function* function);
    bool pass2(Function* function);
    bool pass3(Function* function);
    bool pass4(Function* function);
    bool pass5(Function* function);
    void fuseBlock(Function* function, BasicBlock* i, BasicBlock* j);
    void checkPhi(Function* function);
    void checkPhiPreds(Function* function);
    void checkAllocAndPhi(Function* function);
    void setOriginBranch(Function* function);
    void checkCond();
};

#endif