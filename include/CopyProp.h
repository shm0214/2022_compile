#ifndef __COPY_PROP_H
#define __COPY_PROP_H

#include <vector>
// #include<tr1/unordered_map>//在unordered_map之前加上tr1库名，
#include <unordered_map>
#include "Unit.h"
#define UOO unordered_map<Operand*, Operand*>
// using namespace std::tr1;
using namespace std;
class CopyProp {
    Unit* unit;

   public:
    CopyProp(Unit* unit) : unit(unit){};
    ~CopyProp();
    void pass();
    void local_copy_prop(BasicBlock* bb, UOO ACP);
};

#endif
