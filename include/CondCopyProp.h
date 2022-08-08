#ifndef __COND_COPY_PROP_H
#define __COND_COPY_PROP_H

#include <vector>
#include<unordered_map>
#include "SSAGraph.h"
#include "SSADestruction.h"
#include "Unit.h"

class CondCopyProp {
    Unit* unit;
   public:
    CondCopyProp(Unit* unit) : unit(unit) {};
    ~CondCopyProp();
    void pass();
};

#endif