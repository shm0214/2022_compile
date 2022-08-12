#include "CopyProp.h"
#include <vector>
using namespace std;
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
        Operand* def = iter->getDef();
        vector<Operand*> uses(iter->getUse());
        for(auto opd : uses) {
            if (!opd->isConst()) {
                Operand* new_opd = copy_value(opd, flag);
                if (flag) {
                    iter->replaceUse(opd, new_opd);
                    if(!opd->usersNum()){
                        bb->remove(opd->getDef());
                    }
                }
            }
        } 
        if(def){
            remove_ACP(ACP, def);
            if(iter->isStore())
                ACP[def] = ((StoreInstruction*)iter)->getSrc();
        }
    }
}

CopyProp::~CopyProp() {
    ACP.clear();
}