#ifndef __GLOBAL2LOCAL_H__
#define __GLOBAL2LOCAL_H__

#include "Unit.h"

using namespace std;

class Global2Local {
    Unit* unit;
    map<SymbolEntry*, map<Function*, vector<Instruction*>>> globals;
    map<Function*, set<SymbolEntry*>> usedGlobals;
    map<Function*, set<SymbolEntry*>> read;
    map<Function*, set<SymbolEntry*>> write;

   public:
    Global2Local(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* function);
    void calGlobals();
    void unstoreGlobal2Const();
};

#endif