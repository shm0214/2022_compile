#ifndef __COPY_PROP_H
#define __COPY_PROP_H

#include <vector>
#include "Unit.h"

class CopyProp {
    Unit* unit;
   public:
    CopyProp(Unit* unit) : unit(unit){};
    void copy_prop();
    void local_copy_prop(BasicBlock* bb);
    void global_copy_prop(BasicBlock* bb);
    
};

#endif