#ifndef __COPY_PROP_H
#define __COPY_PROP_H

#include <vector>
#include<unordered_map>
#include "Unit.h"
#define UOO unordered_map<Operand*, Operand*>
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
