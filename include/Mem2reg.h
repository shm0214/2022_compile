#ifndef __MEM2REG_H__
#define __MEM2REG_H__

#include "Unit.h"

class Mem2reg {
    Unit* unit;
    std::vector<AllocaInstruction*> allocaIns;
    std::map<Operand*, std::stack<Operand*>> stacks;
    std::vector<BinaryInstruction*> addZeroIns;

   public:
    Mem2reg(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* function);
    void insertPhiInstruction(Function* function);
    void rename(BasicBlock* block);
    void rename(Function* function);
    Operand* newName(Operand* old);
    void cleanAddZeroIns(Function* function);
    void checkCondBranch(Function* function);
};
#endif