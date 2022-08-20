#include "Mem2reg.h"
#include <map>
#include <set>
#include <vector>
#include "BasicBlock.h"
#include "Instruction.h"
using namespace std;

void Mem2reg::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void Mem2reg::pass(Function* function) {
    checkCondBranch(function);
    function->computeDFSTree();
    function->computeSdom();
    function->computeIdom();
    function->computeDomFrontier();
    insertPhiInstruction(function);
    rename(function);
    // 这个会导致r0-r3被覆盖
    cleanAddZeroIns(function);
}

void Mem2reg::insertPhiInstruction(Function* function) {
    vector<BinaryInstruction*>().swap(addZeroIns);
    vector<AllocaInstruction*>().swap(allocaIns);
    BasicBlock* entry = function->getEntry();
    for (auto i = entry->begin(); i != entry->end(); i = i->getNext()) {
        if (!i->isAlloc())
            break;
        auto alloca = (AllocaInstruction*)i;
        if (!alloca->isArray())
            allocaIns.push_back(alloca);
    }
    vector<BasicBlock*> worklist;
    set<BasicBlock*> inWorklist, inserted, assigns;
    for (auto i : allocaIns) {
        vector<BasicBlock*>().swap(worklist);
        inWorklist.clear();
        inserted.clear();
        assigns.clear();
        auto block = i->getParent();
        block->remove(i);
        auto operand = i->getDef();
        operand->setDef(nullptr);
        Operand* newOperand = new Operand(new TemporarySymbolEntry(
            ((PointerType*)(operand->getType()))->getType(),
            SymbolTable::getLabel()));
        i->replaceDef(newOperand);
        // 目前一个operand只有一个def
        // calculate all assign
        // auto uses = operand->ge
        // int num = operand->use_end() - operand->use_begin();
        // // Operand* lastDef = nullptr;
        // for (int i = 0; i < num; i++) {
        while (operand->use_begin() != operand->use_end()) {
            auto use = operand->use_begin();
            if ((*use)->isStore()) {
                // 不知道对不对
                // i = i + 1
                // 正常修改operand  需要 先 load 然后修改 再store
                // 如果store的dst addr 是operand
                // 那么看一下前一条dst是不是store的src
                // 若是 则之金额修改前一条dst为operand
                // if (((StoreInstruction*)(*use))->getDstAddr() == operand) {
                //     lastDef = ((StoreInstruction*)(*use))->getSrc();
                // }
                if (newOperand != (*use)->getUse()[1]) {
                    auto assignIns = new BinaryInstruction(
                        BinaryInstruction::ADD, newOperand, (*use)->getUse()[1],
                        new Operand(
                            new ConstantSymbolEntry(newOperand->getType(), 0)));
                    addZeroIns.push_back(assignIns);
                    (*use)->getParent()->insertBefore(assignIns, *use);
                    assigns.insert((*use)->getParent());
                    (*use)->getUse()[1]->removeUse(*use);
                }
            }
            auto dst = (*use)->getDef();
            (*use)->getParent()->remove(*use);
            if (dst && dst != newOperand)
                while (dst->use_begin() != dst->use_end()) {
                    auto u = *(dst->use_begin());
                    // if (lastDef) {
                    //     u->replaceUse(dst, lastDef);
                    //     lastDef = nullptr;
                    // } else
                    u->replaceUse(dst, newOperand);
                }
            operand->removeUse(*use);
        }
        for (auto block : assigns) {
            worklist.push_back(block);
            inWorklist.insert(block);
            while (!worklist.empty()) {
                BasicBlock* n = worklist[0];
                worklist.erase(worklist.begin());
                for (auto m : n->domFrontier) {
                    if (inserted.find(m) == inserted.end()) {
                        m->insertPhiInstruction(newOperand);
                        inserted.insert(m);
                        if (inWorklist.find(m) == inWorklist.end()) {
                            inWorklist.insert(m);
                            worklist.push_back(m);
                        }
                    }
                }
            }
        }
    }
}

void Mem2reg::rename(Function* function) {
    stacks.clear();
    for (auto i : allocaIns) {
        auto operand = i->getDef();
        stacks[operand] = stack<Operand*>();
        //delete i;
    }
    rename(function->getEntry());
}

void Mem2reg::rename(BasicBlock* block) {
    std::map<Operand*, int> counter;
    for (auto i = block->begin(); i != block->end(); i = i->getNext()) {
        Operand* def = i->getDef();
        if (def && stacks.find(def) != stacks.end()) {
            counter[def]++;
            Operand* new_ = newName(def);
            i->replaceDef(new_);
        }
        if (!i->isPhi())
            for (auto u : i->getUse())
                if (stacks.find(u) != stacks.end() && !stacks[u].empty())
                    i->replaceUse(u, stacks[u].top());
    }
    for (auto it = block->succ_begin(); it != block->succ_end(); it++) {
        for (auto i = (*it)->begin(); i != (*it)->end(); i = i->getNext()) {
            if (i->isPhi()) {
                PhiInstruction* phi = (PhiInstruction*)i;
                Operand* o = phi->getOriginDef();
                if (!stacks[o].empty())
                    phi->addSrc(block, stacks[o].top());
                else
                    phi->addSrc(block, new Operand(new ConstantSymbolEntry(
                                           o->getType(), 0)));
            } else
                break;
        }
    }
    auto func = block->getParent();
    auto node = func->getDomNode(block);
    for (auto child : node->children)
        rename(child->block);
    for (auto it : counter)
        for (int i = 0; i < it.second; i++)
            stacks[it.first].pop();
}

Operand* Mem2reg::newName(Operand* old) {
    Operand* ret = new Operand(*old);
    stacks[old].push(ret);
    return ret;
}

void Mem2reg::cleanAddZeroIns(Function* func) {
    auto type = (FunctionType*)(func->getSymPtr()->getType());
    int paramNo = type->getParamsType().size() - 1;
    int regNum = 4;
    if (paramNo > 3)
        regNum--;
    for (auto i : addZeroIns) {
        auto use = i->getUse()[0];
        // if (use->getEntry()->isConstant())
        //     continue;
        if (i->getParent()->begin() == i && i->getNext()->isUncond())
            continue;
        if (use->getEntry()->isVariable()) {
            continue;
            // if (func->hasCall())
            //     if (paramNo < regNum) {
            //         paramNo--;
            //         continue;
            //     }
            // if (paramNo >= regNum) {
            //     paramNo--;
            //     continue;
            // }
            // paramNo--;
        }
        auto def = i->getDef();
        // if (def != use)
        while (def->use_begin() != def->use_end()) {
            auto u = *(def->use_begin());
            u->replaceUse(def, use);
        }
        i->getParent()->remove(i);
        use->removeUse(i);
        delete i;
    }
}

void Mem2reg::checkCondBranch(Function* func) {
    for (auto block : func->getBlockList()) {
        auto in = block->rbegin();
        if (in->isCond()) {
            auto cond = (CondBrInstruction*)in;
            auto trueBlock = cond->getTrueBranch();
            auto falseBlock = cond->getFalseBranch();
            if (trueBlock == falseBlock) {
                block->removeSucc(trueBlock);
                trueBlock->removePred(block);
                new UncondBrInstruction(trueBlock, block);
                block->remove(in);
            }
        }
    }
}