#ifndef __DIV_TO_MUL_H
#define __DIV_TO_MUL_H

#include "Unit.h"
class Div2Mul {
    Unit* unit;
   public:
    Div2Mul(Unit* unit) : unit(unit){};
    void pass();
    void div2mul(Function *);
    ~Div2Mul();
}