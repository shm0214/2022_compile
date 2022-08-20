#ifndef __PEEPHOLE_OPTIMIZATION_H__
#define __PEEPHOLE_OPTIMIZATION_H__

#include "MachineCode.h"

class PeepholeOptimization {
   public:
    PeepholeOptimization(MachineUnit* mUnit);
    void pass();
    void pass1();
   private:
    MachineUnit* mUnit;
};

#endif  // __PEEPHOLE_OPTIMIZATION_H__