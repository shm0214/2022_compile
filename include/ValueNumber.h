#ifndef __VALUE_NUMBER_H__
#define __VALUE_NUMBER_H__

#include "Unit.h"

class ValueNumber {
    Unit* unit;

   public:
    ValueNumber(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* func);
    void pass(BasicBlock* block,
              std::map<std::string, Operand*> hash,
              std::map<Operand*, Operand*> valueNumber,
              std::set<Operand*> stores,
              std::set<SymbolEntry*> storeGlobals);
    void calFuncStoreGlobals();
};

#endif