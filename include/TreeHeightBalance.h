#ifndef __TREE_HEIGHT_BALANCE_H__
#define __TREE_HEIGHT_BALANCE_H__

#include <queue>
#include "LVAForIR.h"
#include "Unit.h"

// #define PAIR std::pair<Operand*, int>

// struct cmp {
//     bool operator()(const PAIR& a, const PAIR& b) {
//         return a.second > b.second;
//     }
// };

// class TreeHeightBalance {
//     Unit* unit;
//     std::map<Operand*, int> rank;
//     LVAForIR lva;
//     std::set<Operand*> rootSet;
//     std::set<Operand*> params;
//     std::set<Instruction*> delList;

//    public:
//     TreeHeightBalance(Unit* unit) : unit(unit){};
//     void pass();
//     void pass(Function* func);
//     void pass(BasicBlock* block);
//     void balance(Operand* root);
//     int flatten(Operand* var,
//                 std::priority_queue<PAIR, std::vector<PAIR>, cmp>& q);
//     void rebuild(Operand* root,
//                  std::priority_queue<PAIR, std::vector<PAIR>, cmp>& q,
//                  Instruction* in);
// };

struct cmp {
    bool operator()(const Operand* a, const Operand* b) const {
        auto name1 = a->toStr();
        auto name2 = b->toStr();
        auto flag1 = a->isConst();
        auto flag2 = b->isConst();
        if (!flag1 && !flag2)
            return a->getLabel() < b->getLabel();
        if (!flag1 && flag2)
            return false;
        if (flag1 && !flag2)
            return true;
        if (flag1 && flag2)
            return a->getConstVal() < b->getConstVal();
        return true;
    }
};

class TreeHeightBalance {
    Unit* unit;

   public:
    TreeHeightBalance(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* func);
    void pass(BasicBlock* block);
    void convert(std::map<Operand*, int, cmp> operands,
                 std::vector<Instruction*> addIns);
};

#endif