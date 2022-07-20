#ifndef __COPY_PROP_H
#define __COPY_PROP_H

#include <vector>
// #include<tr1/unordered_map>//在unordered_map之前加上tr1库名，
#include<unordered_map>
#include "Unit.h"
#define USS unordered_map<SymbolEntry*, SymbolEntry*>
#define USO unordered_map<SymbolEntry*, Operand*>
// using namespace std::tr1;
using namespace std;
class CopyProp {
    Unit* unit;
   public:
    CopyProp(Unit* unit) : unit(unit){};
    ~CopyProp();
    void copy_prop();
    void local_copy_prop(BasicBlock* bb, USS ACP);
};

#endif