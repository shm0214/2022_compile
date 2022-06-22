#include "Starighten.h"

using namespace std;

void Starighten::pass() {
    auto iter = unit->begin();
    while (iter != unit->end()) {
        changes.clear();
        pass1(*iter);
        //pass2(*iter);
        pass3(*iter);
        checkPhi(*iter);
        iter++;
    }
}

void Starighten::pass1(Function* func) {
    auto& blocklist = func->getBlockList();
    for (auto i : blocklist) {
        if (i->getNumOfSucc() == 1 &&
            (*(i->succ_begin()))->getNumOfPred() == 1) {
            assert(*((*(i->succ_begin()))->pred_begin()) == i);
            auto j = *(i->succ_begin());
            fuseBlock(func, i, j);
            if (changes.find(j) == changes.end())
                changes.insert(make_pair(j, vector<BasicBlock*>()));
            changes[j].push_back(i);
            pass1(func);
        }
    }
}
void Starighten::pass2(Function* func) {
    // 这里同时删除了顺序连接的基本块之间的无条件跳转
    // 目前找不到打印时的下一个基本块
    auto blocklist = func->getBlockList();
    for (auto it = blocklist.begin(); it != blocklist.end(); it++) {
        auto i = *it;
        if (i->getNumOfSucc() == 1 && i->rbegin()->isUncond()) {
            if (it + 1 == blocklist.end())
                continue;
            auto nextBB = *(it + 1);
            auto jumpBB = ((UncondBrInstruction*)(i->rbegin()))->getBranch();
            if (nextBB == jumpBB)
                i->remove(i->rbegin());
        }
    }
}
void Starighten::pass3(Function* func) {
    // 删除只有一句无条件跳转的基本块
    auto& blocklist = func->getBlockList();
    for (auto it = blocklist.begin(); it != blocklist.end();) {
        if ((*it)->begin() == (*it)->rbegin() && (*it)->begin()->isUncond()) {
            auto block = ((UncondBrInstruction*)((*it)->begin()))->getBranch();
            block->removePred(*it);
            if ((*it)->getNumOfPred())
                changes.insert(make_pair(*it, vector<BasicBlock*>()));
            for (auto it1 = (*it)->pred_begin(); it1 != (*it)->pred_end();
                 it1++) {
                auto ins = (*it1)->rbegin();
                if (ins->isCond()) {
                    auto condIns = (CondBrInstruction*)ins;
                    if (condIns->getTrueBranch() == *it)
                        condIns->setTrueBranch(block);
                    if (condIns->getFalseBranch() == *it)
                        condIns->setFalseBranch(block);
                } else if (ins->isUncond()) {
                    auto unCondIns = (UncondBrInstruction*)ins;
                    if (unCondIns->getBranch() == *it)
                        unCondIns->setBranch(block);
                }
                (*it1)->removeSucc(*it);
                (*it1)->addSucc(block);
                block->addPred(*it1);
                changes[*it].push_back(*it1);
            }
            it = blocklist.erase(it);
        } else {
            it++;
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

void Starighten::checkPhi(Function* func) {
    auto& blocklist = func->getBlockList();
    for (auto it = blocklist.begin(); it != blocklist.end(); it++) {
        (*it)->cleanPhiBlocks();
        for (auto i = (*it)->begin(); i != (*it)->end(); i = i->getNext())
            if (i->isPhi())
                ((PhiInstruction*)i)->changeSrcBlock(changes);
            else
                break;
    }
}