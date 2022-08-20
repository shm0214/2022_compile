#include "PeepholeForIR.h"

using namespace std;

void PeepholeForIR::pass() {
    auto iter = unit->begin();
    while (iter != unit->end()) {
        pass1(*iter);
        pass2(*iter);
        pass3(*iter);
        iter++;
    }
    cleanOnlyStore();
}

void PeepholeForIR::pass1(Function* func) {
    bool again = true;
    while (again) {
        again = false;
        for (auto block : func->getBlockList()) {
            auto cur = block->begin();
            auto next = cur->getNext();
            vector<Instruction*> rmvList;
            vector<Instruction*> addList;
            while (next != block->end()) {
                if (cur->isAlloc()) {
                    auto addr = cur->getDef();
                    if (addr->getType()->isPtr2Array()) {
                        vector<Instruction*> stores;
                        vector<Instruction*> loads;
                        bool flag = true;
                        for (auto it = addr->use_begin(); it != addr->use_end();
                             it++) {
                            auto in = *it;
                            if (in->isStore())
                                stores.push_back(in);
                            else if (in->isLoad())
                                loads.push_back(in);
                            else {
                                flag = false;
                                break;
                            }
                        }
                        if (flag && !stores.empty()) {
                            if (stores.size() != 1) {
                                cout << "stores size != 1\n";
                                assert(0);
                            }
                            auto storeSrc = stores[0]->getUse()[1];
                            if (storeSrc->isParam()) {
                                auto newSrc =
                                    new Operand(new TemporarySymbolEntry(
                                        storeSrc->getType(),
                                        SymbolTable::getLabel()));
                                auto bitcast =
                                    new BitcastInstruction(newSrc, storeSrc);
                                bitcast->setParent(block);
                                addList.push_back(bitcast);
                                // block->insertAfter(bitcast, cur);
                                // next = bitcast;
                                storeSrc = newSrc;
                            }
                            auto arrType =
                                ((PointerType*)(addr->getType()))->getType();
                            if (storeSrc->getType()->toStr() !=
                                arrType->toStr()) {
                                auto newSrc =
                                    new Operand(new TemporarySymbolEntry(
                                        arrType, SymbolTable::getLabel()));
                                auto bitcast =
                                    new BitcastInstruction(newSrc, storeSrc);
                                bitcast->setParent(block);
                                block->insertBefore(bitcast, cur);
                                storeSrc = newSrc;
                            }
                            for (auto in : loads) {
                                auto loadDef = in->getDef();
                                while (loadDef->use_begin() !=
                                       loadDef->use_end()) {
                                    auto use = *(loadDef->use_begin());
                                    use->replaceUse(loadDef, storeSrc);
                                }
                            }
                            rmvList.push_back(cur);
                            rmvList.push_back(stores[0]);
                            rmvList.insert(rmvList.end(), loads.begin(),
                                           loads.end());
                        }
                    }
                } else if (cur->isConstExp()) {
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
            for (auto it = addList.rbegin(); it != addList.rend(); it++) {
                auto in = *it;
                auto block = in->getParent();
                for (auto i = block->begin(); i != block->end();
                     i = i->getNext()) {
                    if (!i->isAlloc()) {
                        block->insertBefore(in, i);
                        break;
                    }
                }
            }
        }
    }
}

void PeepholeForIR::cleanOnlyStore() {
    set<string> loads;
    map<string, vector<Instruction*>> stores;
    for (auto iter = unit->begin(); iter != unit->end(); iter++)
        for (auto block : (*iter)->getBlockList())
            for (auto in = block->begin(); in != block->end();
                 in = in->getNext()) {
                if (in->isLoad()) {
                    auto addr = in->getUse()[0];
                    if (addr->isGlobal())
                        loads.insert(addr->toStr());
                }
                if (in->isStore()) {
                    auto addr = in->getUse()[0];
                    if (addr->isGlobal())
                        stores[addr->toStr()].push_back(in);
                }
            }
    for (auto it : stores)
        if (!loads.count(it.first))
            for (auto in : it.second) {
                auto use = in->getUse()[1];
                use->removeUse(in);
                in->getParent()->remove(in);
            }
}

void PeepholeForIR::pass2(Function* func) {
    for (auto block : func->getBlockList()) {
        auto in = block->rbegin();
        if (!in->isCond())
            continue;
        auto cmp = (CmpInstruction*)(in->getPrev());
        auto uses = cmp->getUse();
        // cmp 冗余时出现
        if (uses.empty())
            continue;
        if (uses[0]->isConst() && uses[0]->getConstVal() == 0)
            cmp->swapSrc();
    }
}

void PeepholeForIR::pass3(Function* func) {
    auto block = func->getEntry();
    Instruction* r3Add = nullptr;
    for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
        if (in->isAddZero()) {
            auto use = in->getUse()[0];
            if (use->isParam()) {
                auto entry = (IdentifierSymbolEntry*)(use->getEntry());
                if (entry->getParamNo() == 3) {
                    r3Add = in;
                    break;
                }
            }
        }
    }
    if (r3Add) {
        block->remove(r3Add);
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            if (!in->isAlloc()) {
                block->insertBefore(r3Add, in);
                break;
            }
        }
    }
}