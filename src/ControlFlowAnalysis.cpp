#include "ControlFlowAnalysis.h"

using namespace std;

void ControlFlowAnalysis::pass(MachineFunction* func) {
    edges.clear();
    visited.clear();
    loopDepth.clear();
    i = 1;
    j = 1;
    for (auto block : func->getBlocks())
        loopDepth[block] = 0;
    dfs(func->getEntry());
    computeLoopDepth();
}

void ControlFlowAnalysis::dfs(MachineBlock* block) {
    visited.insert(block);
    preOrder[block] = j++;
    for (auto s : block->getSuccs()) {
        if (visited.find(s) == visited.end()) {
            dfs(s);
            edges[make_pair(block, s)] = TREE;
        } else if (preOrder[block] < preOrder[s])
            edges[make_pair(block, s)] = FORWARD;
        else if (postOrder.find(s) == postOrder.end())
            edges[make_pair(block, s)] = BACK;
        else
            edges[make_pair(block, s)] = CROSS;
    }
    postOrder[block] = i++;
}

set<MachineBlock*> ControlFlowAnalysis::naturalLoop(MachineBlock* b1,
                                                    MachineBlock* b2) {
    set<MachineBlock*> loop;
    stack<MachineBlock*> stk;
    loop.insert(b1);
    loop.insert(b2);
    if (b1 != b2)
        stk.push(b1);
    while (!stk.empty()) {
        auto p = stk.top();
        stk.pop();
        for (auto q : p->getPreds()) {
            if (loop.find(q) == loop.end()) {
                loop.insert(q);
                stk.push(q);
            }
        }
    }
    return loop;
}

void ControlFlowAnalysis::computeLoopDepth() {
    for (auto e : edges)
        if (e.second == BACK) {
            // 这个BACK并不是严格意义的回边 先这样吧
            auto loop = naturalLoop(e.first.first, e.first.second);
            for (auto b : loop)
                loopDepth[b]++;
        }
}