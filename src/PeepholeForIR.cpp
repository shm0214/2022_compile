#include "PeepholeForIR.h"

using namespace std;

void PeepholeForIR::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void PeepholeForIR::pass(Function* func) {
    for (auto block : func->getBlockList()) {
        auto cur = block->begin();
        auto next = cur->getNext();
        vector<Instruction*> rmvList;
        while (next != block->end()) {
            if (cur->isStore() && next->isLoad()) {
                auto strAddr = cur->getUse()[0];
                auto ldrAddr = next->getUse()[0];
                auto flag = strAddr == ldrAddr;
                if (strAddr->isGlobal() && ldrAddr->isGlobal())
                    flag = strAddr->toStr() == ldrAddr->toStr();
                if (flag) {
                    auto strSrc = cur->getUse()[1];
                    auto ldrDef = next->getDef();
                    while (ldrDef->use_begin() != ldrDef->use_end()) {
                        auto use = *(ldrDef->use_begin());
                        use->replaceUse(ldrDef, strSrc);
                    }
                    ldrAddr->removeUse(next);
                    rmvList.push_back(next);
                }
            }
            cur = next;
            next = next->getNext();
        }
        for (auto in : rmvList)
            in->getParent()->remove(in);
    }
}