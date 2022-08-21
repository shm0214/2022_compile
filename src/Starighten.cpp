#include "Starighten.h"

using namespace std;

void Starighten::pass() {
    auto iter = unit->begin();
    while (iter != unit->end()) {
        bool again = true;
        while (again) {
            again = false;
            checkPhiPreds(*iter);
            setOriginBranch(*iter);
            // (*iter)->output();
            changes.clear();
            again = pass1(*iter) || again;
            //pass2(*iter);
            again = pass3(*iter) || again;
            // (*iter)->output();
            again = pass5(*iter) || again;
            // (*iter)->output();
            again = pass4(*iter) || again;
            // (*iter)->output();
            // for (auto it : changes)
            //     cout << it.first->getNo() << "->" << it.second[0]->getNo()
            //          << endl;
            checkPhi(*iter);
            // (*iter)->output();
            checkPhiPreds(*iter);
            // checkAllocAndPhi(*iter);
        }
        iter++;
    }
}

bool Starighten::pass1(Function* func) {
    bool ret = false;
    auto& blocklist = func->getBlockList();
    for (auto i : blocklist) {
        if (i->getNumOfSucc() == 1 &&
            (*(i->succ_begin()))->getNumOfPred() == 1) {
            ret = true;
            assert(*((*(i->succ_begin()))->pred_begin()) == i);
            auto j = *(i->succ_begin());
            fuseBlock(func, i, j);
            if (changes.find(j) == changes.end())
                changes.insert(make_pair(j, vector<BasicBlock*>()));
            changes[j].push_back(i);
            pass1(func);
        }
    }
    return ret;
}
bool Starighten::pass2(Function* func) {
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
    return false;
}
bool Starighten::pass3(Function* func) {
    // 删除只有一句无条件跳转的基本块
    bool ret = false;
    auto& blocklist = func->getBlockList();
    for (auto it = blocklist.begin(); it != blocklist.end();) {
        if ((*it)->begin() == (*it)->rbegin() && (*it)->begin()->isUncond()) {
            if ((*it) == (*it)->getParent()->getEntry()) {
                it++;
                continue;
            }
            auto unCond = (UncondBrInstruction*)((*it)->begin());
            if (unCond->isNoStraighten()) {
                it++;
                continue;
            }
            ret = true;
            auto block = unCond->getBranch();
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
                auto succs = (*it1)->getSucc();
                bool first = succs[0] == *it;
                (*it1)->removeSucc(*it);
                (*it1)->addSucc(block, first);
                block->addPred(*it1);
                changes[*it].push_back(*it1);
            }
            it = blocklist.erase(it);
        } else {
            it++;
        }
    }
    return ret;
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
        it->setParent(i);
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

void Starighten::checkPhiPreds(Function* func) {
    auto& blocklist = func->getBlockList();
    vector<Instruction*> removeIns;
    for (auto it = blocklist.begin(); it != blocklist.end(); it++) {
        auto preds = (*it)->getPred();
        for (auto i = (*it)->begin(); i != (*it)->end(); i = i->getNext())
            if (i->isPhi()) {
                auto phi = (PhiInstruction*)i;
                auto& srcs = phi->getSrcs();
                vector<BasicBlock*> rmvList;
                for (auto it : srcs)
                    if (find(preds.begin(), preds.end(), it.first) ==
                        preds.end()) {
                        rmvList.push_back(it.first);
                        phi->removeUse(it.second);
                        it.second->removeUse(phi);
                    }
                for (auto block : rmvList)
                    srcs.erase(block);
                if (srcs.size() == 1) {
                    auto def = phi->getDef();
                    auto use = phi->getUse()[0];
                    use->removeUse(phi);
                    // 要退化为add 0
                    // auto zero = new Operand(
                    //     new ConstantSymbolEntry(TypeSystem::intType, 0));
                    // auto addIn = new BinaryInstruction(BinaryInstruction::ADD,
                    //                                    def, use, zero);
                    // addIn->setParent(*it);
                    // replaceIns[phi] = addIn;
                    // 直接替换所有def即可吧
                    while (def->use_begin() != def->use_end()) {
                        auto in = *(def->use_begin());
                        in->replaceUse(def, use);
                    }
                    removeIns.push_back(phi);
                }
            }
    }
    for (auto it : removeIns)
        it->getParent()->remove(it);
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

bool Starighten::pass4(Function* func) {
    // 解决由于DCE带来的无前驱块
    bool ret = false;
    // auto& blocklist = func->getBlockList();
    // vector<BasicBlock*> temp;
    // for (auto it = blocklist.begin(); it != blocklist.end(); it++) {
    //     if ((*it)->getNumOfPred() == 0 && *it != func->getEntry()) {
    //         for (auto it1 = (*it)->pred_begin(); it1 != (*it)->pred_end();
    //              it1++) {
    //             (*it1)->removePred(*it);
    //         }
    //         temp.push_back(*it);
    //         ret = true;
    //     }
    // }
    // for (auto b : temp)
    //     func->remove(b);
    // 需要改进成移除所有非enter所在的联通分量内的块
    auto& blocklist = func->getBlockList();
    set<BasicBlock*> blocks;
    for (auto block : blocklist)
        blocks.insert(block);
    set<BasicBlock*> visited;
    stack<BasicBlock*> stk;
    stk.push(func->getEntry());
    while (!stk.empty()) {
        auto block = stk.top();
        stk.pop();
        if (visited.count(block))
            continue;
        visited.insert(block);
        for (auto succ : block->getSucc())
            stk.push(succ);
    }
    set<BasicBlock*> temp;
    set_difference(blocks.begin(), blocks.end(), visited.begin(), visited.end(),
                   inserter(temp, temp.end()));
    if (temp.size())
        ret = true;
    for (auto block : temp) {
        for (auto succ = block->succ_begin(); succ != block->succ_end(); succ++)
            (*succ)->removePred(block);
    }
    for (auto block : temp)
        func->remove(block);
    return ret;
}

void Starighten::checkAllocAndPhi(Function* func) {
    // 解决由于内联导致的alloc 和 phi不在块的开始
}

void Starighten::setOriginBranch(Function* func) {
    for (auto block : func->getBlockList()) {
        auto in = block->rbegin();
        if (in->isCond()) {
            auto cond = (CondBrInstruction*)in;
            cond->setOriginFalse(cond->getFalseBranch());
            cond->setOriginTrue(cond->getTrueBranch());
        }
    }
}

bool Starighten::pass5(Function* func) {
    bool ret = false;
    // auto again = true;
    // while (again) {
    //     again = false;
    BasicBlock *first, *last = nullptr;
    Operand* condSrc;
    bool branch;
    for (auto block : func->getBlockList()) {
        auto in = block->rbegin();
        if (!in->isCond())
            continue;
        auto cond = (CondBrInstruction*)in;
        condSrc = cond->getUse()[0];
        auto succs = block->getSucc();
        for (int i = 0; i < 2; i++) {
            if (succs[i] == block)
                continue;
            if (succs[i]->getNumOfPred() != 1)
                continue;
            auto in = succs[i]->rbegin();
            if (in->isCond()) {
                auto use = in->getUse()[0];
                if (use == condSrc) {
                    first = block;
                    last = succs[i];
                    branch = cond->getTrueBranch() == succs[i] ? true : false;
                    break;
                }
            }
        }
        if (last)
            break;
    }
    if (last) {
        // again = true;
        ret = true;
        CondBrInstruction* cond;

        auto temp = first;
        while (true) {
            BasicBlock *succ, *other;
            cond = (CondBrInstruction*)(temp->rbegin());
            if (branch) {
                succ = cond->getTrueBranch();
                other = cond->getFalseBranch();
            } else {
                succ = cond->getFalseBranch();
                other = cond->getTrueBranch();
            }
            if (temp != first) {
                temp->removeSucc(other);
                other->removePred(temp);
                new UncondBrInstruction(succ, temp);
                temp->remove(cond);
            }
            if (temp == last)
                break;
            temp = succ;
        }

        // BasicBlock* succ;
        // while (true) {
        //     cond = (CondBrInstruction*)(last->rbegin());
        //     if (branch)
        //         succ = cond->getTrueBranch();
        //     else
        //         succ = cond->getFalseBranch();
        //     auto in = succ->rbegin();
        //     if (in->isCond()) {
        //         auto use = in->getUse()[0];
        //         if (use == condSrc)
        //             last = succ;
        //         else
        //             break;

        //     } else
        //         break;
        // }
        // succ->removePred(last);
        // BasicBlock* second;
        // auto firstCond = (CondBrInstruction*)(first->rbegin());
        // if (branch)
        //     second = firstCond->getTrueBranch();
        // else
        //     second = firstCond->getFalseBranch();
        // first->removeSucc(second);
        // second->removePred(first);
        // if (branch)
        //     firstCond->setTrueBranch(succ);
        // else
        //     firstCond->setFalseBranch(succ);
        // first->addSucc(succ);
        // succ->addPred(first);
        // changes[last].push_back(first);
    }
    // }
    return ret;
}

void Starighten::checkCond() {
    for (auto it = unit->begin(); it != unit->end(); it++)
        for (auto block : (*it)->getBlockList()) {
            auto in = block->rbegin();
            if (!in->isCond())
                continue;
            if (!in->getPrev()->isCmp()) {
                auto condSrc = in->getUse()[0];
                auto cmp = condSrc->getDef();
                auto newCmp = cmp->copy();
                for (auto u : newCmp->getUse())
                    u->addUse(newCmp);
                auto cmpDst = new Operand(new TemporarySymbolEntry(
                    TypeSystem::boolType, SymbolTable::getLabel()));
                newCmp->setDef(cmpDst);
                in->replaceUse(condSrc, cmpDst);
                newCmp->setParent(block);
                block->insertBefore(newCmp, in);
            }
        }
}