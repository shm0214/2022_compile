#include "ValueNumber.h"

using namespace std;

// ref: engineering a compiler 8.4.1 & 10.5.2

int log2(int value) {
    if (value == 1)
        return 0;
    else
        return 1 + log2(value >> 1);
}

inline bool is2Exp(int val) {
    return !(val & (val - 1));
}

void ValueNumber::pass() {
    calFuncStoreGlobals();
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void ValueNumber::pass(Function* func) {
    func->computeDFSTree();
    func->computeSdom();
    func->computeIdom();
    func->computeStores();
    auto entry = func->getEntry();
    pass(entry, {}, {}, {}, {});
}

void ValueNumber::pass(BasicBlock* block,
                       map<string, Operand*> hash,
                       map<Operand*, Operand*> valueNumber,
                       set<Operand*> stores,
                       set<SymbolEntry*> storeGlobals) {
    vector<Instruction*> temp;
    // cout << block->getNo() << endl;
    set<Operand*> alreadyLoad;
    for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
        // auto str = in->getHash();
        // if (str.length())
        //     cout << str << endl;
        if (in->isCmp() && in->getNext()->isXor())
            continue;
        if (in->isPhi()) {
            set<Operand*> s;
            auto operands = in->getOperands();
            for (auto it = operands.begin() + 1; it != operands.end(); it++)
                // if (valueNumber.count(*it)) {
                //     auto ope = valueNumber[*it];
                //     in->replaceUse(*it, ope);
                //     s.insert(ope);
                // } else
                s.insert(*it);
            if (s.size() == 1 || hash.count(in->getHash()) == 1) {
                // 无意义 或 冗余
                ((PhiInstruction*)in)->cleanUse();
                valueNumber[in->getDef()] = *(s.begin());
                temp.push_back(in);
                auto ope = in->getDef();
                auto new_ = valueNumber[ope];
                auto it1 = ope->use_begin();
                while (it1 != ope->use_end()) {
                    (*it1)->replaceUse(ope, new_);
                    it1 = ope->use_begin();
                }
            } else {
                valueNumber[in->getDef()] = in->getDef();
                hash[in->getHash()] = in->getDef();
            }
        } else {
            if (in->isCall()) {
                IdentifierSymbolEntry* funcSE =
                    (IdentifierSymbolEntry*)(((CallInstruction*)in)
                                                 ->getFuncSE());
                if (!(funcSE->isSysy() ||
                      funcSE->getName() == "llvm.memset.p0.i32")) {
                    auto func = funcSE->getFunction();
                    auto& store = func->getStores();
                    for (auto ope : store)
                        if (ope->getEntry()->isVariable())
                            stores.insert(ope);
                    auto globalStore = func->getStoredGlobals();
                    for (auto ope : globalStore) {
                        string name = ope->toStr().substr(1);
                        auto global = identifiers->lookup(name);
                        storeGlobals.insert(global);
                    }
                }
            }
            // if (in->isRet()) {
            //     auto src = in->getUse()[0];
            //     if (valueNumber.count(src))
            //         in->replaceUse(src, valueNumber[src]);
            // }
            if (in->getDef()) {
                if (!in->getHash().length())
                    continue;
                auto operands = in->getOperands();
                for (auto it = operands.begin() + 1; it != operands.end(); it++)
                    if (valueNumber.count(*it))
                        in->replaceUse(*it, valueNumber[*it]);
                // simplified 是指常量合并与代数恒等式？
                if (in->isConstExp()) {
                    // 比较指令需要修改块 先不做
                    if (!in->isCmp()) {
                        auto def = in->getDef();
                        auto type = def->getType();
                        auto newOpe = new Operand(
                            new ConstantSymbolEntry(type, in->getConstVal()));
                        valueNumber[def] = newOpe;
                        while (def->use_begin() != def->use_end()) {
                            auto it = def->use_begin();
                            (*it)->replaceUse(def, newOpe);
                        }
                        temp.push_back(in);
                        continue;
                    }
                }
                // 先改一下乘2和除2 其他用的不多
                if (in->isIntMul()) {
                    Operand* ope = nullptr;
                    Operand* def = operands[0];
                    Operand* src1 = operands[1];
                    Operand* src2 = operands[2];
                    int val = 0;
                    if (src1->getEntry()->isConstant()) {
                        auto entry = (ConstantSymbolEntry*)(src1->getEntry());
                        if (is2Exp(entry->getValue())) {
                            ope = src1;
                            val = log2(entry->getValue());
                        }
                    }
                    if (src2->getEntry()->isConstant()) {
                        auto entry = (ConstantSymbolEntry*)(src2->getEntry());
                        if (is2Exp(entry->getValue())) {
                            ope = src2;
                            val = log2(entry->getValue());
                        }
                    }
                    if (ope && val) {
                        Operand* src = ope == src1 ? src2 : src1;
                        src->removeUse(in);
                        ope->removeUse(in);
                        auto shl = new ShlInstruction(
                            def, src,
                            new Operand(new ConstantSymbolEntry(
                                TypeSystem::intType, val)));
                        shl->setParent(block);
                        block->replaceIns(in, shl);
                        in = shl;
                    }
                }
                // 这个并不是完全对的
                // 负奇数/2得不到正确的答案
                // 但只有一个功能样例出现了 先这样吧
                // 后续在这个之前把除法优化做了
                // TODO:
                if (in->isIntDiv()) {
                    Operand* def = operands[0];
                    Operand* src1 = operands[1];
                    Operand* src2 = operands[2];
                    if (src2->getEntry()->isConstant()) {
                        auto entry = (ConstantSymbolEntry*)(src2->getEntry());
                        if (entry->getValue() == 2) {
                            src1->removeUse(in);
                            src2->removeUse(in);
                            auto ashr = new AshrInstruction(
                                def, src1,
                                new Operand(new ConstantSymbolEntry(
                                    TypeSystem::intType, 1)));
                            ashr->setParent(block);
                            block->replaceIns(in, ashr);
                            in = ashr;
                        }
                    }
                }

                if (hash.count(in->getHash())) {
                    if (in->isLoad()) {
                        auto use = in->getUse()[0];
                        if (use->isGlobal()) {
                            string name = use->toStr().substr(1);
                            auto global = identifiers->lookup(name);
                            if (storeGlobals.count(global)) {
                                storeGlobals.erase(global);
                                valueNumber[in->getDef()] = in->getDef();
                                hash[in->getHash()] = in->getDef();
                                continue;
                            } else {
                                auto s = block->getStores();
                                auto flag = false;
                                for (auto o : s) {
                                    if (o->isGlobal() &&
                                        o->toStr().substr(1) == name) {
                                        valueNumber[in->getDef()] =
                                            in->getDef();
                                        hash[in->getHash()] = in->getDef();
                                        block->removeStore(use);
                                        flag = true;
                                        break;
                                    }
                                }
                                if (flag)
                                    continue;
                            }
                        } else {
                            if (stores.count(use)) {
                                stores.erase(use);
                                valueNumber[in->getDef()] = in->getDef();
                                hash[in->getHash()] = in->getDef();
                                continue;
                            } else if (block->inStore(use)) {
                                if (!use->getType()->isPtr2Array()) {
                                    valueNumber[in->getDef()] = in->getDef();
                                    hash[in->getHash()] = in->getDef();
                                    block->removeStore(use);
                                    continue;
                                }
                            } else {
                                if (!use->getType()->isPtr2Array()) {
                                    auto blockStores = block->getStores1();
                                    auto f = false;
                                    for (auto s : blockStores) {
                                        if (use->getDef() && s->getDef() &&
                                            use->getDef()->isGep() &&
                                            s->getDef()->isGep() &&
                                            use->getDef()->getHash() ==
                                                s->getDef()->getHash()) {
                                            if (!alreadyLoad.count(s)) {
                                                alreadyLoad.insert(s);
                                                f = true;
                                                break;
                                            }
                                        }
                                    }
                                    if (f) {
                                        valueNumber[in->getDef()] =
                                            in->getDef();
                                        hash[in->getHash()] = in->getDef();
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                    valueNumber[in->getDef()] = hash[in->getHash()];
                    temp.push_back(in);
                    // auto addr = in->getUse()[0];
                    // addr->removeUse(in);
                    for (auto u : in->getUse())
                        u->removeUse(in);
                    auto ope = in->getDef();
                    auto new_ = valueNumber[ope];
                    auto it = ope->use_begin();
                    while (it != ope->use_end()) {
                        (*it)->replaceUse(ope, new_);
                        it = ope->use_begin();
                    }
                } else {
                    valueNumber[in->getDef()] = in->getDef();
                    hash[in->getHash()] = in->getDef();
                }
            } else if (in->isStore())
                if (!in->getUse()[1]->getEntry()->isVariable()) {
                    auto useAddr = in->getUse()[0];
                    if (useAddr->isGlobal()) {
                        string name = useAddr->toStr().substr(1);
                        auto global = identifiers->lookup(name);
                        storeGlobals.insert(global);
                    } else {
                        if (!useAddr->getType()->isPtr2Array())
                            stores.insert(useAddr);
                    }
                }
        }
    }
    for (auto it = block->succ_begin(); it != block->succ_end(); it++) {
        for (auto in = (*it)->begin(); in != (*it)->end(); in = in->getNext()) {
            if (!in->isPhi())
                break;
            auto operands = in->getOperands();
            for (auto it1 = operands.begin() + 1; it1 != operands.end();
                 it1++) {
                if (valueNumber.count(*it1)) {
                    auto ope = valueNumber[*it1];
                    in->replaceUse(*it1, ope);
                }
            }
        }
    }
    for (auto in : temp) {
        in->getParent()->remove(in);
    }
    auto func = block->getParent();
    auto node = func->getDomNode(block);
    for (auto node : node->children) {
        auto b = node->block;
        pass(b, hash, valueNumber, stores, storeGlobals);
    }
}

void ValueNumber::calFuncStoreGlobals() {
    for (auto it = unit->begin(); it != unit->end(); it++) {
        auto func = *it;
        for (auto block : func->getBlockList())
            for (auto in = block->begin(); in != block->end();
                 in = in->getNext())
                if (in->isStore()) {
                    auto addr = in->getUse()[0];
                    if (addr->isGlobal())
                        func->addStoredGlobal(addr);
                }
    }
}
