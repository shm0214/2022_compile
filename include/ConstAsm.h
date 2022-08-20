#ifndef __CONST_ASM_H__
#define __CONST_ASM_H__

#include <map>
#include "MachineCode.h"

class ConstAsm {
   public:
    ConstAsm(MachineUnit* mUnit);
    void pass();

   private:
    MachineUnit* mUnit;

    std::map<int, MachineInstruction*> op2inst;
    std::map<int, int> op2imm;

    void simplify(MachineBlock* block);
};

#endif  // __CONST_ASM_H__