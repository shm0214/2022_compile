#include "PeepholeForIR.h"

using namespace std;

void PeepholeForIR::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void PeepholeForIR::pass(Function* func) {
    bool again = true;
    while (again) {
        again = false;
        for (auto block : func->getBlockList()) {
            auto cur = block->begin();
            auto next = cur->getNext();
            vector<Instruction*> rmvList;
            while (next != block->end()) {
                if (cur->isConstExp()) {
                    // xor性能应该不会出现 cmp出现的话处理复杂一些
                    if (!(cur->isCmp() || cur->isXor())) {
                        double val = cur->getConstVal();
                        auto def = cur->getDef();
                        auto imm = new Operand(
                            new ConstantSymbolEntry(def->getType(), val));
                        while (def->use_begin() != def->use_end()) {
                            auto use = *(def->use_begin());
                            use->replaceUse(def, imm);
                        }
                        rmvList.push_back(cur);
                    }
                } else if (cur->isAddZero()) {
                    auto src = cur->getUse()[0];
                    if (!src->isParam()) {
                        auto def = cur->getDef();
                        while (def->use_begin() != def->use_end()) {
                            auto use = *(def->use_begin());
                            use->replaceUse(def, src);
                        }
                        src->removeUse(cur);
                        rmvList.push_back(cur);
                    }
                } else if (cur->isStore() && next->isLoad()) {
                    auto strAddr = cur->getUse()[0];
                    auto ldrAddr = next->getUse()[0];
                    auto flag = strAddr == ldrAddr;
                    if (strAddr->isGlobal() && ldrAddr->isGlobal())
                        flag = strAddr->toStr() == ldrAddr->toStr();
                    if (flag) {
                        auto strSrc = cur->getUse()[1];
                        if (!strSrc->isParam()) {
                            auto ldrDef = next->getDef();
                            while (ldrDef->use_begin() != ldrDef->use_end()) {
                                auto use = *(ldrDef->use_begin());
                                use->replaceUse(ldrDef, strSrc);
                            }
                            ldrAddr->removeUse(next);
                            rmvList.push_back(next);
                        }
                    }
                }
                cur = next;
                next = next->getNext();
            }
            if (rmvList.size())
                again = true;
            for (auto in : rmvList)
                in->getParent()->remove(in);
        }
    }
}