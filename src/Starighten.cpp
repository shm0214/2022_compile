#include "Starighten.h"

using namespace std;

void Starighten::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void Starighten::pass(Function* func) {
    auto& blocklist = func->getBlockList();
    for (auto i : blocklist) {
        if (i->getNumOfSucc() == 1 &&
            (*(i->succ_begin()))->getNumOfPred() == 1) {
            assert(*((*(i->succ_begin()))->pred_begin()) == i);
            auto j = *(i->succ_begin());
            fuseBlock(func, i, j);
            //pass(func);
        }
    }
}

void Starighten::fuseBlock(Function* func, BasicBlock* i, BasicBlock* j) {
    Instruction* tail = i->rbegin();
    if (tail->isUncond()) {
        tail = tail->getPrev();
        i->remove(tail->getNext());
    }
    for (auto it = j->begin(); it != j->end();) {
        auto temp = it->getNext();
        i->insertBack(it);
        it = temp;
    }
    tail = i->rbegin();
    if (!tail->isUncond() && !tail->isCond()) {
        // 书上说如果没有这种后继，补充一个并返回其值
        // 为什么要补充一个呢 这种一般都是返回了
        if (j->succ_begin() != j->succ_end()) {
            auto block = *(j->succ_begin());
            new UncondBrInstruction(block);
        }
    }
    i->cleanSucc();
    for (auto it = j->succ_begin(); it != j->succ_end(); it++) {
        i->addSucc(*it);
        (*it)->removePred(j);
        (*it)->addPred(i);
    }
    func->remove(j);
}