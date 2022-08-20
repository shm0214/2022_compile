#include "SSADestruction.h"
#include <vector>
using namespace std;

void SSADestruction::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void SSADestruction::pass(Function* function) {
    map<BasicBlock*, vector<Instruction*>> copyInss;
    vector<BasicBlock*> blocks(function->begin(), function->end());
    for (auto it = blocks.begin(); it != blocks.end(); it++) {
        if (!(*it)->begin()->isPhi())
            continue;
        vector<BasicBlock*> preds((*it)->pred_begin(), (*it)->pred_end());
        for (auto pred : preds) {
            if (pred->getNumOfSucc() == 1) {
                for (auto i = (*it)->begin(); i != (*it)->end();
                     i = i->getNext()) {
                    if (!i->isPhi())
                        break;
                    auto def = i->getDef();
                    auto src = ((PhiInstruction*)i)->getSrc(pred);
                    auto copyIns = new BinaryInstruction(
                        BinaryInstruction::ADD, def, src,
                        new Operand(
                            new ConstantSymbolEntry(def->getType(), 0)));
                    // auto endIns = pred->rbegin();
                    // pred->insertBefore(copyIns, endIns);
                    copyInss[pred].push_back(copyIns);
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
                for (auto i = (*it)->begin(); i != (*it)->end();
                     i = i->getNext()) {
                    if (!i->isPhi())
                        break;
                    auto def = i->getDef();
                    auto src = ((PhiInstruction*)i)->getSrc(pred);
                    auto copyIns = new BinaryInstruction(
                        BinaryInstruction::ADD, def, src,
                        new Operand(
                            new ConstantSymbolEntry(def->getType(), 0)));
                    // newBlock->insertFront(copyIns);
                    copyInss[newBlock].push_back(copyIns);
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
    // 修改加入的顺序
    for (auto it : copyInss) {
        auto block = it.first;
        auto& ins = it.second;
        set<Operand*> defs;
        for (auto in : ins)
            defs.insert(in->getDef());
        vector<Instruction*> temp;
        for (auto it = ins.begin(); it != ins.end();) {
            if (defs.count((*it)->getUse()[0]) == 0) {
                temp.push_back(*it);
                it = ins.erase(it);
            } else
                it++;
        }
        for (auto in : ins) {
            bool flag = false;
            auto def = in->getDef();
            for (auto it = temp.begin(); it != temp.end(); it++) {
                if ((*it)->getUse()[0] == def) {
                    temp.insert(it + 1, in);
                    flag = true;
                    break;
                }
            }
            if (flag)
                continue;
            temp.insert(temp.begin(), in);
        }
        auto endIns = block->rbegin();
        for (auto in : temp)
            block->insertBefore(in, endIns);
    }
}