#include "SpecialOptimize.h"
#include <queue>

using namespace std;

void SpecialOptimize::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void SpecialOptimize::pass1() {
    auto iter = unit->begin();
    while (iter != unit->end()) {
        checkOperandsUse(*iter);
        pass1(*iter);
        iter++;
    }
}

void SpecialOptimize::pass(Function* function) {
    // 处理循环对一维数组赋0值 bitset
    // while (i < n) {
    //     res[i] = 0;
    //     i = i + 1;
    // }
    vector<BasicBlock*> rmvList;
    for (auto block : function->getBlockList()) {
        auto in = block->begin();
        if (!in->isPhi())
            continue;
        auto uses = in->getUse();
        bool flag = false;
        auto phiDef = in->getDef();
        for (auto u : uses)
            if (u->isConst() && u->getConstVal() == 0) {
                flag = true;
                break;
            }
        if (!flag)
            continue;
        in = in->getNext();
        if (!in->isLoad())
            continue;
        auto loadDef = in->getDef();
        auto loadUse = in->getUse()[0];
        in = in->getNext();
        if (!in->isGep())
            continue;
        uses = in->getUse();
        if (uses[0] != loadDef || uses[1] != phiDef)
            continue;
        auto gepDef = in->getDef();
        in = in->getNext();
        if (!in->isStore())
            continue;
        uses = in->getUse();
        if (uses[0] != gepDef ||
            !(uses[1]->isConst() && uses[1]->getConstVal() == 0))
            continue;
        in = in->getNext();
        uses = in->getUse();
        if (!in->isAdd())
            continue;
        if (uses[0] != phiDef ||
            !(uses[1]->isConst() && uses[1]->getConstVal() == 1))
            continue;
        auto preds = block->getPred();
        if (preds.size() != 2)
            continue;
        auto pred = preds[0] == block ? preds[1] : preds[0];
        auto succs = block->getSucc();
        if (succs.size() != 2)
            continue;
        auto succ = succs[0] == block ? succs[1] : succs[0];
        rmvList.push_back(block);
        auto cond = pred->rbegin();
        auto cmp = cond->getPrev();
        pred->remove(cond);
        pred->remove(cmp);
        auto n = cmp->getUse()[1];
        auto shlDst = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        auto two = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 2));
        new ShlInstruction(shlDst, n, two, pred);
        auto loadDst = new Operand(new TemporarySymbolEntry(
            new PointerType(TypeSystem::intType), SymbolTable::getLabel()));
        new LoadInstruction(loadDst, loadUse, pred);
        auto int8PtrType = new PointerType(TypeSystem::int8Type);
        Operand* int8Ptr = new Operand(
            new TemporarySymbolEntry(int8PtrType, SymbolTable::getLabel()));
        auto bitcast = new BitcastInstruction(int8Ptr, loadDst, pred);
        bitcast->setFlag();
        std::string name = "llvm.memset.p0.i32";
        auto funcSE = identifiers->lookup(name);
        if (!funcSE) {
            vector<Type*> vec;
            vec.push_back(int8PtrType);
            vec.push_back(TypeSystem::int8Type);
            vec.push_back(TypeSystem::intType);
            vec.push_back(TypeSystem::boolType);
            vector<SymbolEntry*> vec1;
            auto funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
            SymbolTable* st = identifiers;
            while (st->getPrev())
                st = st->getPrev();
            funcSE = new IdentifierSymbolEntry(funcType, name, st->getLevel());
            st->install(name, funcSE);
            unit->insertDeclare(funcSE);
        }
        vector<Operand*> params;
        params.push_back(int8Ptr);
        params.push_back(
            new Operand(new ConstantSymbolEntry(TypeSystem::int8Type, 0)));
        params.push_back(shlDst);
        params.push_back(
            new Operand(new ConstantSymbolEntry(TypeSystem::boolType, 0)));
        new CallInstruction(nullptr, funcSE, params, pred);
        new UncondBrInstruction(succ, pred);
    }
    for (auto block : rmvList) {
        for (auto pred : block->getPred())
            pred->removeSucc(block);
        for (auto succ : block->getSucc())
            succ->removePred(block);
        function->remove(block);
    }
}

struct cmp {
    bool operator()(pair<Operand*, int> a, pair<Operand*, int> b) {
        return a.second < b.second;
    }
};

void SpecialOptimize::pass1(Function* func) {
    // crypto
    for (auto block : func->getBlockList()) {
        map<Operand*, map<Operand*, int>> maps;
        if (block->begin()->getNext() == block->rbegin())
            continue;
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            if (!(in->isAdd() || in->isSub() || in->isShl()))
                break;
            auto def = in->getDef();
            if (!def->getType()->isInt())
                break;
            auto flag = false;
            for (auto it = def->use_begin(); it != def->use_end(); it++) {
                if ((*it)->getParent() != block &&
                    in->getNext() != block->rbegin()) {
                    flag = true;
                    break;
                }
            }
            if (flag)
                break;
            maps[def] = {};
            auto uses = in->getUse();
            if (in->isShl()) {
                auto src = uses[0];
                if (uses[1]->isConst()) {
                    int num = uses[1]->getConstVal();
                    for (auto it : maps[src])
                        maps[def][it.first] += (1 << num) * it.second;
                }
            } else {
                for (int i = 0; i < 2; i++) {
                    auto use = uses[i];
                    if (!use->getDef() || use->getDef()->getParent() != block) {
                        if (i == 1 && in->isSub())
                            maps[def][use]--;
                        else
                            maps[def][use]++;
                    } else if (use->getDef()->getParent() == block) {
                        for (auto it : maps[use]) {
                            if (i == 1 && in->isSub())
                                maps[def][it.first] -= it.second;
                            else
                                maps[def][it.first] += it.second;
                        }
                    }
                }
            }
            if (in->getNext() == block->rbegin()) {
                priority_queue<pair<Operand*, int>, vector<pair<Operand*, int>>,
                               cmp>
                    que;
                for (auto it : maps[def])
                    if (it.second)
                        que.push({it.first, it.second});
                if (que.empty()) {
                    auto zero = new Operand(
                        new ConstantSymbolEntry(TypeSystem::intType, 0));
                    while (def->use_begin() != def->use_end())
                        (*(def->use_begin()))->replaceUse(def, zero);
                    while (block->begin() != block->rbegin())
                        block->remove(block->begin());
                } else if (que.size() == 3) {
                    auto pr1 = que.top();
                    que.pop();
                    auto pr2 = que.top();
                    que.pop();
                    auto pr3 = que.top();
                    que.pop();
                    if (pr1.second == 1 && pr2.second == 1 &&
                        pr3.second == -1) {
                        auto tempDst = new Operand(new TemporarySymbolEntry(
                            TypeSystem::intType, SymbolTable::getLabel()));
                        auto tempDst1 = new Operand(new TemporarySymbolEntry(
                            TypeSystem::intType, SymbolTable::getLabel()));
                        auto add = new BinaryInstruction(BinaryInstruction::ADD,
                                                         tempDst, pr1.first,
                                                         pr2.first);
                        auto sub =
                            new BinaryInstruction(BinaryInstruction::SUB,
                                                  tempDst1, tempDst, pr3.first);
                        add->setParent(block);
                        sub->setParent(block);
                        while (def->use_begin() != def->use_end())
                            (*(def->use_begin()))->replaceUse(def, tempDst1);
                        while (block->begin() != block->rbegin())
                            block->remove(block->begin());
                        block->insertFront(sub);
                        block->insertFront(add);
                    } else
                        continue;
                } else
                    continue;
            }
        }
    }
}

void SpecialOptimize::checkOperandsUse(Function* func) {
    set<BasicBlock*> blocks;
    for (auto block : func->getBlockList())
        blocks.insert(block);
    for (auto block : func->getBlockList()) {
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            auto def = in->getDef();
            if (def) {
                vector<Instruction*> rmvList;
                for (auto it = def->use_begin(); it != def->use_end(); it++)
                    if (!blocks.count((*it)->getParent()))
                        rmvList.push_back(*it);
                for (auto u : rmvList)
                    def->removeUse(u);
            }
        }
    }
}