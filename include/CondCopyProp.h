#ifndef __COND_COPY_PROP_H
#define __COND_COPY_PROP_H

#include <vector>
#include<unordered_map>
#include "Unit.h"
using namespace std;

class CondCopyProp {
    Unit* unit;
   public:
    CondCopyProp(Unit* unit) : unit(unit){};
    ~CondCopyProp();
    void cond_copy_prop();
};

#endif