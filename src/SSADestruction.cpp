#include "SSADestruction.h"
#include <vector>
using namespace std;

void SSADestruction::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void SSADestruction::pass(Function* function) {
    for (auto it = function->begin(); it != function->end(); it++) {
        if (!(*it)->begin()->isPhi())
            continue;
        vector<BasicBlock*> preds((*it)->pred_begin(), (*it)->pred_end());
        for (auto pred : preds) {
            if (pred->getNumOfSucc() == 1) {
                for (auto i = (*it)->begin(); i != (*it)->end(); i = i->getNext()) {
                    if (!i->isPhi())
                        break;
                    auto def = i->getDef();
                    auto src = ((PhiInstruction*)i)->getSrc(pred);
                    auto copyIns = new BinaryInstruction(
                        BinaryInstruction::ADD, def, src,
                        new Operand(
                            new ConstantSymbolEntry(TypeSystem::intType, 0)));
                    auto endIns = pred->rbegin();
                    pred->insertBefore(copyIns, endIns);
                }
            } else {
                BasicBlock* newBlock = new BasicBlock(function);
                new UncondBrInstruction(*it, newBlock);
                newBlock->addSucc(*it);
                (*it)->addPred(newBlock);
                (*it)->removePred(pred);
                CondBrInstruction* branch =
                    (CondBrInstruction*)(pred->rbegin());
                if (branch->getTrueBranch() == *it)
                    branch->setTrueBranch(newBlock);
                else
                    branch->setFalseBranch(newBlock);
                auto succs = pred->getSucc();
                bool first = succs[0] == *it;
                pred->removeSucc(*it);
                pred->addSucc(newBlock, first);
                newBlock->addPred(pred);
                for (auto i = (*it)->begin(); i != (*it)->end(); i=i->getNext()) {
                    if (!i->isPhi())
                        break;
                    auto def = i->getDef();
                    auto src = ((PhiInstruction*)i)->getSrc(pred);
                    auto copyIns = new BinaryInstruction(
                        BinaryInstruction::ADD, def, src,
                        new Operand(
                            new ConstantSymbolEntry(TypeSystem::intType, 0)));
                    newBlock->insertFront(copyIns);
                }
            }
        }
        while ((*it)->begin() != (*it)->end()) {
            auto i = (*it)->begin();
            if (i->isPhi()) {
                (*it)->remove(i);
                // delete i;
            } else
                break;
        }
    }
}