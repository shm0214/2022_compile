#ifndef __INS_REORDER_H__
#define __INS_REORDER_H__

#include "Unit.h"

using namespace std;

struct InsNode {
    Instruction* in;
    bool visited;
    std::vector<InsNode*> children;
    std::vector<InsNode*> parents;
    InsNode(Instruction* in) : in(in), visited(false) {}
    void addChild(InsNode* child) {
        children.push_back(child);
        child->parents.push_back(this);
    }
};

class InsReorder {
    Unit* unit;

   public:
    InsReorder(Unit* unit) : unit(unit){};
    void pass();
    void pass(Function* func);
    void pass(BasicBlock* block);
    void dfs(InsNode* node, BasicBlock* block);
};

#endif