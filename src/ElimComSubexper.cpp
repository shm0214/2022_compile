#include <queue>
#include "ElimComSubexpr.h"
typedef vector<BasicBlock*>::iterator bb_iterator;
using namespace std;
bool ste[10000000];
unordered_map<int, VAEB> AEin, AEout;

//两个vector求交集
VAEB vec_intersection(VAEB v1, VAEB v2) {
    VAEB v11, v22, res;
    int len = min(v1.size(), v2.size());
    if (v1.size() > v2.size()) {
        v11 = v2;
        v22 = v1;
    } else {
        v11 = v1;
        v22 = v2;
    }
    VAEB::iterator it;
    for (int i = 0; i < len; i++) {
        it = find(v22.begin(), v22.end(), v11[i]);
        if (it != v22.end()) {
            res.push_back(v11[i]);
        }
    }
    return res;
}

//两个vector求并集
VAEB vec_union(VAEB v1, VAEB v2) {
    VAEB res(v1);
    VAEB::iterator it;
    for (unsigned i = 0; i < v2.size(); i++) {
        it = find(v1.begin(), v1.end(), v2[i]);
        if (it == v1.end()) {
            res.push_back(v2[i]);
        }
    }
    return res;
}

VAEB vintersection(BasicBlock* bb) {
    VAEB res;
    bb_iterator iter = bb->pred_begin();
    bb_iterator end = bb->pred_end();
    while (iter != end) {
        int no = (*iter)->getNo();
        if (ste[no]) {
            res = vec_intersection(res, AEout[no]);
        }
        iter++;
    }
    return res;
}

void delAEB(Operand* opd, VAEB& AEB) {
    // cout<<"del: "<<opd->toStr()<<endl;
    vector<aeb> rmvList;
    for (auto iter = AEB.begin(); iter != AEB.end(); ++iter) {
        if (iter->isIn(opd)) {
            rmvList.push_back(*iter);
        }
    }
    for (auto aeb : rmvList)
        AEB.erase(find(AEB.begin(), AEB.end(), aeb));
}

void ElimComSubexpr::pass() {
    auto iter = unit->begin();
    VAEB AEB;
    while (iter != unit->end()) {
        BasicBlock* bb = (*iter)->getEntry();
        queue<BasicBlock*> q;
        q.push(bb);
        bool first = true;
        while (!q.empty()) {
            BasicBlock* bb = q.front();
            q.pop();
            int no = bb->getNo();
            if (ste[no])
                continue;
            ste[no] = true;
            if (first) {
                AEin[no];
                first = false;
            } else {
                AEin[no] = vintersection(bb);
            }
            local_elim_cse(bb, AEB);
            for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++) {
                if (!ste[(*succ)->getNo()]) {
                    q.push(*succ);
                }
            }
        }
        iter++;
    }
    // global
    iter = unit->begin();
    AEB.clear();
    while (iter != unit->end()) {
        vector<BasicBlock*> block_list = (*iter)->getBlockList();
        for (auto bb : block_list) {
            AEB = AEin[bb->getNo()];
            local_elim_cse(bb, AEB);
        }
        iter++;
    }
}

void ElimComSubexpr::local_elim_cse(BasicBlock* bb, VAEB AEB) {
    int no = bb->getNo();
    for (auto iter = bb->begin(); iter != bb->end(); iter = iter->getNext()) {
        Operand* def = iter->getDef();
        vector<Operand*> uses(iter->getUse());
        if (uses.size() == 2 && !iter->isCall()) {
            struct aeb t;
            t.inst = iter, t.opd1 = uses[0], t.opr = iter->getOpcode(),
            t.opd2 = uses[1];
            bool found = false;
            int i = 0;
            for (; i < int(AEB.size()); i++) {
                if (t == AEB[i]) {
                    found = true;
                    break;
                }
            }

            if (found) {
                Instruction* p = AEB[i].inst;
                Operand* dst = AEB[i].tmp;
                if (dst == nullptr) {
                    dst = new Operand(new TemporarySymbolEntry(
                        def->getType(), SymbolTable::getLabel()));
                    Instruction* inst = p->copy();
                    inst->replaceDef(dst);
                    AEB[i].tmp = dst;
                    bb->insertBefore(inst, p);
                    Instruction* inst1 =
                        new StoreInstruction(p->getDef(), dst, nullptr);
                    bb->insertBefore(inst1, p);
                    bb->remove(p);
                }
                Instruction* inst2 = new StoreInstruction(def, dst, nullptr);
                bb->insertBefore(inst2, iter);
                bb->remove(iter);
            } else {
                AEB.push_back(t);
            }
        }
        if (def)
            delAEB(def, AEB);
    }
    AEout[no] = vec_union(AEB, AEin[no]);
    AEB.clear();
}

ElimComSubexpr::~ElimComSubexpr() {
    AEin.clear(), AEout.clear();
}
