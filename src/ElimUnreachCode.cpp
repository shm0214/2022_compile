#include "ElimUnreachCode.h"
#include <algorithm>
using namespace std;

void ElimUnreachCode::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void ElimUnreachCode::pass(Function* func) {
    auto blocks = getReachBlocks(func, 0);
    auto& blockList = func->getBlockList();
    int len = blockList.size();
    bool again = false;
    int i;
    for (i = 1; i < len; i++) {
        if (find(blocks.begin(), blocks.end(), i) == blocks.end()) {
            again = true;
            break;
        }
    }
    if (again) {
        BasicBlock* block = blockList[i];
        for (auto iter = block->pred_begin(); iter != block->pred_end(); iter++)
            (*iter)->removeSucc(block);
        for (auto iter = block->succ_begin(); iter != block->succ_end(); iter++)
            (*iter)->removePred(block);
        blockList.erase(blockList.begin() + i);
        pass(func);
    }
}

vector<int> ElimUnreachCode::getReachBlocks(Function* func, int idx) {
    auto m = func->getBlockMap();
    int len = m.size();
    vector<bool> visited(len, false);
    stack<int> stk;
    stk.push(idx);
    int v;
    while (!stk.empty()) {
        v = stk.top();
        stk.pop();
        if (!visited[v])
            visited[v] = true;
        for (int i = 0; i < len; i++)
            if (m[v][i] && !visited[i])
                stk.push(i);
    }
    vector<int> ret;
    for (int i = 0; i < len; i++)
        if (visited[i] && i != idx)
            ret.push_back(i);
    return ret;
}