#include "DeadCodeElimination.h"
#include <vector>

using namespace std;

void DeadCodeElimination::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void DeadCodeElimination::mark(Function* func) {
    vector<Instruction*> worklist;
    for (auto it = func->begin(); it != func->end(); it++) {
        (*it)->unsetMark();
        (*it)->cleanMark();
        for (auto it1 = (*it)->begin(); it1 != (*it)->end();
             it1 = it1->getNext()) {
            if (it1->isEssential()) {
                it1->setMark();
                it1->getParent()->setMark();
                worklist.push_back(it1);
            }
        }
    }
    func->computeReverseDomFrontier();
    while (!worklist.empty()) {
        auto ins = worklist.back();
        worklist.pop_back();
        auto uses = ins->getUse();
        for (auto it : uses) {
            auto def = it->getDef();
            if (def && !def->isMark()) {
                def->setMark();
                def->getParent()->setMark();
                worklist.push_back(def);
            }
        }
        auto def = ins->getDef();
        if (def) {
            for (auto use = def->use_begin(); use != def->use_end(); use++) {
                if (!(*use)->isMark() &&
                    ((*use)->isUncond() || (*use)->isCond())) {
                    (*use)->setMark();
                    (*use)->getParent()->setMark();
                    worklist.push_back(*use);
                }
            }
        }
        auto block = ins->getParent();
        for (auto b : block->domFrontier) {
            Instruction* in = b->rbegin();
            if (!in->isMark() && (in->isCond() || in->isUncond())) {
                in->setMark();
                in->getParent()->setMark();
                worklist.push_back(in);
            }
        }
    }
}

bool DeadCodeElimination::remove(Function* func) {
    vector<Instruction*> temp;
    bool ret = false;
    for (auto& block : func->getBlockList()) {
        for (auto it = block->begin(); it != block->end(); it = it->getNext()) {
            if (!it->isMark()) {
                if (!it->isUncond())
                    temp.push_back(it);
                if (it->isCond()) {
                    BasicBlock* b = func->getMarkBranch(block);
                    auto branch = new UncondBrInstruction(b, block);
                    for (auto it1 = block->succ_begin();
                         it1 != block->succ_end(); it1++)
                        (*it1)->removePred(block);
                    block->addSucc(b);
                    b->addPred(block);
                }
            }
        }
    }
    if (temp.size())
        ret = true;
    for (auto i : temp) {
        i->getParent()->remove(i);
    }
    return ret;
}

void DeadCodeElimination::pass(Function* func) {
    bool again = true;
    while (again) {
        mark(func);
        remove(func);
        again = false;
    }
}