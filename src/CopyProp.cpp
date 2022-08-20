#include "CopyProp.h"
#include <queue>
#include <vector>
typedef vector<BasicBlock*>::iterator bb_iterator;
using namespace std;
bool st[10000000];
unordered_map<int, UOO> CPin, CPout;

UOO map_intersection(UOO u1, UOO u2) {
    UOO res;
    for (auto i : u1) {
        if (u2.find(i.first) != u2.end()) {
            if (u2[i.first] == i.second) {
                res[i.first] = i.second;
            }
        }
    }
    return res;
}

UOO map_union(UOO u1, UOO u2) {
    UOO res;
    for (auto v : u1) {
        res[v.first] = v.second;
    }
    for (auto v : u2) {
        res[v.first] = v.second;
    }
    return res;
}

//多个map求交集
UOO intersection(BasicBlock* bb) {
    UOO res;
    bb_iterator iter = bb->pred_begin();
    bb_iterator end = bb->pred_end();
    vector<UOO> tmp;
    while (iter != end) {
        int no = (*iter)->getNo();
        if (st[no]) {
            res = map_intersection(res, CPout[no]);
        }
        iter++;
    }
    return res;
}

void remove_ACP(UOO& ACP, Operand* opd) {
    for (auto iter = ACP.begin(); iter != ACP.end(); ++iter) {
        if (iter->first == opd || iter->second == opd) {
            ACP.erase(iter);
        }
    }
}

Operand* copy_value(Operand* opd, UOO& ACP, bool& flag) {
    for (auto iter = ACP.begin(); iter != ACP.end(); iter++) {
        if (opd == iter->first) {
            flag = true;
            return iter->second;
        }
    }
    flag = false;
    return opd;
}

void CopyProp::pass() {
    auto iter = unit->begin();
    UOO ACP;
    while (iter != unit->end()) {
        BasicBlock* bb = (*iter)->getEntry();
        queue<BasicBlock*> q;
        q.push(bb);
        bool first = true;
        while (!q.empty()) {
            BasicBlock* bb = q.front();
            q.pop();
            int no = bb->getNo();
            if (st[no])
                continue;
            st[no] = true;
            if (first) {
                CPin[no];
                first = false;
            } else {
                CPin[no] = intersection(bb);
            }
            local_copy_prop(bb, ACP);
            for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++) {
                if (!st[(*succ)->getNo()]) {
                    q.push(*succ);
                }
            }
        }
        iter++;
    }
    // global_copy_prop
    iter = unit->begin();
    ACP.clear();
    while (iter != unit->end()) {
        vector<BasicBlock*> block_list = (*iter)->getBlockList();
        for (auto bb : block_list) {
            ACP = CPin[bb->getNo()];
            local_copy_prop(bb, ACP);
        }
        iter++;
    }
}

void CopyProp::local_copy_prop(BasicBlock* bb, UOO ACP) {
    int no = bb->getNo();
    bool flag;
    for (auto iter = bb->begin(); iter != bb->end(); iter = iter->getNext()) {
        Operand* def = iter->getDef();
        vector<Operand*> uses(iter->getUse());
        for (auto opd : uses) {
            if (!opd->isConst()) {
                Operand* new_opd = copy_value(opd, ACP, flag);
                if (flag) {
                    iter->replaceUse(opd, new_opd);
                    if (!opd->usersNum()) {
                        // opd->getDef()->output();
                        bb->remove(opd->getDef());
                    }
                }
            }
        }
        if (def) {
            remove_ACP(ACP, def);
            if (iter->isStore())
                ACP[def] = ((StoreInstruction*)iter)->getSrc();
        }
    }

    CPout[no] = map_union(ACP, CPin[no]);
    ACP.clear();
}

CopyProp::~CopyProp() {
    CPin.clear(), CPout.clear();
}
