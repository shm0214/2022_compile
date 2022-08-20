#include "ElimUnreachCode.h"
#include <algorithm>
using namespace std;

void ElimUnreachCode::pass() {
    auto iter = unit->begin();
    while (iter != unit->end()) {
        pass1(*iter);
        pass2(*iter);
        iter++;
    }
}

void ElimUnreachCode::pass1(Function* func) {
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
        pass1(func);
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

void ElimUnreachCode::pass2(Function* func) {
    // 删除由于常量传播等造成的常数条件if/while
    map<Instruction*, bool> cmpIns;
    for (auto block : func->getBlockList()) {
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            if (in->isCmp()) {
                auto uses = in->getUse();
                if (uses[0]->isConst() && uses[1]->isConst()) {
                    auto val1 = uses[0]->getConstVal();
                    auto val2 = uses[1]->getConstVal();
                    bool result;
                    switch (in->getOpcode()) {
                        case CmpInstruction::E:
                            result = val1 == val2;
                            break;
                        case CmpInstruction::NE:
                            result = val1 != val2;
                            break;
                        case CmpInstruction::L:
                            result = val1 < val2;
                            break;
                        case CmpInstruction::LE:
                            result = val1 <= val2;
                            break;
                        case CmpInstruction::G:
                            result = val1 > val2;
                            break;
                        case CmpInstruction::GE:
                            result = val1 >= val2;
                            break;
                    }
                    cmpIns[in] = result;
                }
            }
        }
    }
    for (auto it : cmpIns) {
        auto in = it.first;
        auto parent = in->getParent();
        auto def = in->getDef();
        // 应该只有一个use吧
        // assert(def->usersNum() == 1);
        // auto useIn = *(def->use_begin());
        // 还需要xor的先不管了  性能里面应该不多
        bool flag = false;
        for (auto it = def->use_begin(); it != def->use_end(); it++)
            if (!(*it)->isCond()) {
                flag = true;
                break;
            }
        if (!flag)
            parent->remove(in);
        for (auto it1 = def->use_begin(); it1 != def->use_end(); it1++) {
            if (!(*it1)->isCond())
                continue;
            auto branch = (CondBrInstruction*)(*it1);
            parent = branch->getParent();
            if (parent->rbegin() != branch)
                continue;
            BasicBlock *dstBlock, *otherBlock;
            if (it.second) {
                dstBlock = branch->getTrueBranch();
                otherBlock = branch->getFalseBranch();
            } else {
                dstBlock = branch->getFalseBranch();
                otherBlock = branch->getTrueBranch();
            }
            new UncondBrInstruction(dstBlock, parent);
            parent->removeSucc(otherBlock);
            otherBlock->removePred(parent);
            parent->remove(branch);
        }
    }
}