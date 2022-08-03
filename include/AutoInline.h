#ifndef __AUTO_INLINE_H__
#define __AUTO_INLINE_H__

#include <map>
#include "Unit.h"

class AutoInline {
    Unit* unit;
    std::map<CallInstruction*, Function*> workList;

   public:
    AutoInline(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* function);
    void decide(CallInstruction* in);
    void deal(CallInstruction* in);
    void calRecursion();
};

#endif