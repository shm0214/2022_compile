#include "CopyProp.h"
#include <queue>
#include <set>
#include <vector>
using namespace std;
typedef vector<BasicBlock*>::iterator bb_iterator;
UOO ACP;

void remove_ACP(UOO& ACP, Operand* opd) {
    for (auto iter = ACP.begin(); iter != ACP.end(); ++iter) {
        if (iter->first == opd || iter->second == opd) {
            ACP.erase(iter);
        }
    }
}

Operand* copy_value(Operand* opd, bool& flag) {
    for (auto iter = ACP.begin(); iter != ACP.end(); iter++) {
        if (opd == iter->first) {
            flag = true;
            return iter->second;
        }
    }
    flag = false;
    return opd;
}


void CopyProp::pass(){
    auto iter = unit->begin();
    iter = unit->begin();
    while (iter != unit->end()){
        vector<BasicBlock*> block_list = (*iter)->getBlockList();
        for(auto bb: block_list){
            local_copy_prop(bb);
        }
        iter++;
    }
}

void CopyProp::local_copy_prop(BasicBlock* bb) {
    bool flag;
    for (auto iter = bb->begin(); iter != bb->end(); iter = iter->getNext()) {
        vector<Operand*> operands(iter->getOperands());
        if (iter->isBin()) {
            if (!operands[1]->isConst()) {
                Operand* opd = copy_value(operands[1], flag);
                if (flag) {
                    iter->replaceUse(operands[1], opd);
                }
            }
            if (!operands[2]->isConst()) {
                Operand* opd = copy_value(operands[2], flag);
                if (flag) {
                    iter->replaceUse(operands[2], opd);
                }
            }
        } else if (iter->isStore()) {
            remove_ACP(ACP, operands[0]);
            if (!operands[1]->isConst()) {
                ACP[operands[0]] = operands[1];
            }
        }
        operands.clear();
    }
}

CopyProp::~CopyProp() {
    ACP.clear();
}