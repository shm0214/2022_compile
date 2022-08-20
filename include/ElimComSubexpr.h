#ifndef __ELIM_COM_SUBEXPR_H
#define __ELIM_COM_SUBEXPR_H

#include <vector>
#include "Unit.h"
#include<unordered_map>
using namespace std;

enum { SUB, ADD, AND, OR, MUL, DIV, MOD };
struct aeb
{
    Instruction* inst;
    Operand* opd1;
    int opr;
    Operand* opd2;
    Operand* tmp = nullptr;
    bool operator< (aeb a) const{
        return opr<a.opr;
    }
    bool operator== (aeb a) const{
        return (inst->hasEqualOp(a.inst) && (opd1==a.opd1 || (opd1->isConst() && a.opd1->isConst() && opd1->getConstVal() == a.opd1->getConstVal()))
        && (opd2==a.opd2 || (opd2->isConst() && a.opd2->isConst() && opd2->getConstVal() == a.opd2->getConstVal())));
    }
    bool isIn(Operand* opd){
        return opd1==opd || opd2==opd;
    }
};
typedef vector<struct aeb> VAEB;

class ElimComSubexpr {
    Unit* unit;
   public:
    ElimComSubexpr(Unit* unit) : unit(unit){};
    ~ElimComSubexpr();
    void pass();
    void local_elim_cse(BasicBlock* bb, VAEB AEB);
};

#endif