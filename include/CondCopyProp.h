#ifndef __COND_COPY_PROP_H
#define __COND_COPY_PROP_H

#include "SSAGraph.h"
#include "Unit.h"

class CondCopyProp
{
    Unit* unit;
public:
    CondCopyProp(Unit* unit): unit(unit) {};
    ~CondCopyProp();
    void pass();
    void constantPropagation(Function *);
};

#endif