#ifndef __ELIM_COM_SUBEXPR_H
#define __ELIM_COM_SUBEXPR_H

#include <vector>
#include "Unit.h"
<<<<<<< HEAD
#include<unordered_map>
=======
#include <unordered_map>
>>>>>>> 5d9f6c660dcdd5eb5fa3ae6b655f106023081260
using namespace std;

enum { SUB, ADD, AND, OR, MUL, DIV, MOD };
struct aeb
{
    BinaryInstruction* inst;
    SymbolEntry* opd1;
    int opr;
    SymbolEntry* opd2;
    Operand* tmp = nullptr;
    bool operator< (aeb a) const{
        return opr<a.opr;
    }
    bool operator== (aeb a) const{
        return inst==a.inst && opr==a.opr && opd1==a.opd1 && opd2==a.opd2;
    }
};

class ElimComSubexpr {
    Unit* unit;
   public:
    ElimComSubexpr(Unit* unit) : unit(unit){};
    ~ElimComSubexpr();
    void pass();
    void local_elim_cse(BasicBlock* bb, vector<struct aeb> AEB);
};

#endif