#ifndef __AUTO_INLINE_H__
#define __AUTO_INLINE_H__

#include <map>
#include "Unit.h"

class AutoInline {
    Unit* unit;
    std::map<CallInstruction*, Function*> workList;
    // std::set<Operand*> allocaDefs;
    std::vector<Function*> inlineFuncs;

   public:
    AutoInline(Unit* unit) : unit(unit), inlineFuncs({}){};
    void pass();
    void pass(Function* function);
    void decide(CallInstruction* in);
    void deal(CallInstruction* in);
    void calRecursion();
    void calInstNum();
    // void dealAlloc(Function* func);
};

#endif