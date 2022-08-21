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
                if (it1->isStore()) {
                    auto addr = it1->getUse()[0];
                    if (!(addr->isParam() || addr->isGlobal()) &&
                        !addr->getDef())
                        continue;
                }
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
        // 增加对于phi前驱的block的保留
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            if (!in->isPhi())
                continue;
            auto phi = (PhiInstruction*)in;
            for (auto it : phi->getSrcs()) {
                Instruction* in = it.first->rbegin();
                if (!in->isMark() && (in->isCond() || in->isUncond())) {
                    in->setMark();
                    in->getParent()->setMark();
                    worklist.push_back(in);
                }
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
                if (it->isRet()) {
                    auto zero = new Operand(
                        new ConstantSymbolEntry(TypeSystem::intType, 0));
                    it->replaceUse(it->getUse()[0], zero);
                    continue;
                }
                if (it->isCall()) {
                    if (it->isEssential())
                        continue;
                    else {
                        IdentifierSymbolEntry* funcSE =
                            (IdentifierSymbolEntry*)(((CallInstruction*)it)
                                                         ->getFuncSE());
                        if (!funcSE->isSysy() &&
                            funcSE->getName() != "llvm.memset.p0.i32") {
                            auto func1 = funcSE->getFunction();
                            func1->removePred(it);
                        }
                    }
                }
                if (!it->isUncond())
                    temp.push_back(it);
                if (it->isCond()) {
                    BasicBlock* b = func->getMarkBranch(block);
                    if (!b)
                        // 这种情况只能是整个函数都没用 所以不处理了
                        return false;
                    new UncondBrInstruction(b, block);
                    block->cleanSucc();
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
        adjustBlock(func);
        mark(func);
        again = remove(func);
        adjustBlock(func);
    }
}

void DeadCodeElimination::adjustBlock(Function* func) {
    // 删除没有前驱的块
    bool again = true;
    while (again) {
        again = false;
        vector<BasicBlock*> temp;
        for (auto block : func->getBlockList())
            if (block->getNumOfPred() == 0 && block != func->getEntry())
                temp.push_back(block);
        if (temp.size())
            again = true;
        for (auto block : temp) {
            delete block;
        }
    }
}