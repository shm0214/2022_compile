#ifndef __LVA_FOR_IR_H__
#define __LVA_FOR_IR_H__

#include <map>
#include <set>
#include "Unit.h"

class LVAForIR {
   private:
    std::map<BasicBlock*, std::set<Operand*>> UEVar;
    std::map<BasicBlock*, std::set<Operand*>> VarKill;
    std::map<BasicBlock*, std::set<Operand*>> LiveOut;
    void init(BasicBlock* block);
    void computeLiveOut(Function* func);

   public:
    void pass(Function* func);
    std::set<Operand*> getUEVar(BasicBlock* block) { return UEVar[block]; }
    std::set<Operand*> getLiveOut(BasicBlock* block) { return LiveOut[block]; }
};

#endif