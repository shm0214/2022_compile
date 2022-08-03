#include "AutoInline.h"
#include <vector>

using namespace std;

// ref: Advanced Compiler Design and Implementation 15.2

Operand* getTempOperand(Operand* ope) {
    return new Operand(
        new TemporarySymbolEntry(ope->getType(), SymbolTable::getLabel()));
}

void AutoInline::pass() {
    // calRecursion();
    calInstNum();
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void AutoInline::pass(Function* func) {
    workList.clear();
    for (auto block : func->getBlockList()) {
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            if (in->isCall()) {
                auto callIn = (CallInstruction*)in;
                decide(callIn);
            }
        }
    }
    for (auto it : workList) {
        deal(it.first);
    }
}

void AutoInline::decide(CallInstruction* in) {
    IdentifierSymbolEntry* funcSE =
        (IdentifierSymbolEntry*)(((CallInstruction*)in)->getFuncSE());
    if (funcSE->isSysy() || funcSE->getName() == "llvm.memset.p0.i32")
        return;
    auto func = funcSE->getFunction();
    // // 无意义的函数不内联 便于dce删除
    // auto preds = func->getPreds();
    // bool flag = true;
    // for (auto it : preds)
    //     for (auto in : it.second) {
    //         auto def = in->getDef();
    //         if (def && def->usersNum())
    //             flag = false;
    //     }
    // if (flag)
    //     return;
    // 递归不内联
    if (func->hasRecur())
        return;
    // 参数 >= 10 内联
    auto type = (FunctionType*)(func->getSymPtr()->getType());
    int size = type->getParamsType().size();
    if (size >= 10) {
        workList[in] = func;
        return;
    }
    int num = func->getInstNum();
    if (num >= 50)
        return;
    // if inline
    workList[in] = func;
}

void AutoInline::deal(CallInstruction* in) {
    if (!workList.count(in))
        return;
    auto func = workList[in];
    auto parent = in->getParent();
    auto ret = in->getDef();
    auto params = in->getUse();
    // ins after callIn move to exit block
    auto exit = new BasicBlock(parent->getParent());
    auto first = in->getNext();
    auto last = parent->rbegin();
    auto head = exit->end();
    head->setNext(first);
    first->setPrev(head);
    last->setNext(head);
    head->setPrev(last);
    for (auto in1 = exit->begin(); in1 != exit->end(); in1 = in1->getNext())
        in1->setParent(exit);
    in->setNext(parent->end());
    parent->end()->setPrev(in);
    while (!parent->succEmpty()) {
        auto succ = *(parent->succ_begin());
        succ->removePred(parent);
        parent->removeSucc(succ);
        succ->addPred(exit);
        exit->addSucc(succ);
        // change phi src block in parent's succ
        map<BasicBlock*, vector<BasicBlock*>> temp;
        temp[parent] = {exit};
        for (auto in1 = succ->begin(); in1 != succ->end();
             in1 = in1->getNext()) {
            if (!in1->isPhi())
                break;
            auto phi = (PhiInstruction*)in1;
            phi->changeSrcBlock(temp);
        }
    }
    // copy func
    BasicBlock* entry;
    map<Operand*, Operand*> ope2ope;
    map<BasicBlock*, BasicBlock*> block2block;
    vector<BasicBlock*> retBlocks;
    vector<Operand*> retOpes;
    vector<Instruction*> branchs;
    // new, old
    map<Instruction*, Instruction*> phis;
    for (auto block : func->getBlockList()) {
        auto newBlock = new BasicBlock(parent->getParent());
        if (block == func->getEntry())
            entry = newBlock;
        block2block[block] = newBlock;
        for (auto in1 = block->begin(); in1 != block->end();
             in1 = in1->getNext()) {
            Instruction* newIn;
            if (in1->isRet()) {
                retBlocks.push_back(newBlock);
                auto uses = in1->getUse();
                if (uses.size()) {
                    auto use = uses[0];
                    auto zero = new Operand(
                        new ConstantSymbolEntry(TypeSystem::intType, 0));
                    auto dst = getTempOperand(use);
                    retOpes.push_back(dst);
                    Operand* src;
                    if (use->isConst())
                        src = use;
                    else if (ope2ope.count(use))
                        src = ope2ope[use];
                    else {
                        src = getTempOperand(use);
                        ope2ope[use] = src;
                    }
                    newIn = new BinaryInstruction(BinaryInstruction::ADD, dst,
                                                  src, zero);
                    newIn->setParent(newBlock);
                    newBlock->insertBack(newIn);
                }
                new UncondBrInstruction(exit, newBlock);
            } else {
                newIn = in1->copy();
                newIn->setParent(newBlock);
                if (newIn->isCall())
                    ((CallInstruction*)newIn)->addPred();
                switch (newIn->getInstType()) {
                    case Instruction::PHI: {
                        phis.insert({newIn, in1});
                        break;
                    }
                    case Instruction::UNCOND:
                    case Instruction::COND: {
                        branchs.push_back(newIn);
                        break;
                    }
                    default: {
                        auto def = in1->getDef();
                        if (def) {
                            Operand* dst;
                            if (ope2ope.count(def))
                                dst = ope2ope[def];
                            else {
                                dst = getTempOperand(def);
                                ope2ope[def] = dst;
                            }
                            newIn->setDef(dst);
                        }
                        auto uses = in1->getUse();
                        for (auto use : uses) {
                            Operand* src;
                            if (use->isGlobal()) {
                                src = use;
                            } else if (use->isParam()) {
                                int no = use->getParamNo();
                                src = params[no];
                            } else if (use->isConst()) {
                                src = use;
                            } else {
                                if (ope2ope.count(use))
                                    src = ope2ope[use];
                                else {
                                    src = getTempOperand(use);
                                    ope2ope[use] = src;
                                }
                            }
                            newIn->replaceUse(use, src);
                        }
                    }
                }
                newBlock->insertBack(newIn);
            }
        }
    }
    for (auto block : func->getBlockList()) {
        auto newBlock = block2block[block];
        for (auto it = block->pred_begin(); it != block->pred_end(); it++)
            newBlock->addPred(block2block[*it]);
        for (auto it = block->succ_begin(); it != block->succ_end(); it++)
            newBlock->addSucc(block2block[*it]);
    }
    for (auto in : branchs) {
        if (in->isCond()) {
            auto use = in->getUse()[0];
            Operand* src;
            if (ope2ope.count(use))
                src = ope2ope[use];
            else {
                src = getTempOperand(use);
                ope2ope[use] = src;
            }
            in->replaceUse(use, src);
            auto cond = (CondBrInstruction*)in;
            cond->setTrueBranch(block2block[cond->getTrueBranch()]);
            cond->setFalseBranch(block2block[cond->getFalseBranch()]);
        }
        if (in->isUncond()) {
            auto unCond = (UncondBrInstruction*)in;
            unCond->setBranch(block2block[unCond->getBranch()]);
        }
    }
    for (auto it : phis) {
        auto oldPhi = (PhiInstruction*)(it.second);
        auto newPhi = (PhiInstruction*)(it.first);
        newPhi->getSrcs().clear();
        for (auto it : oldPhi->getSrcs()) {
            Operand* src;
            auto use = it.second;
            if (use->isConst()) {
                src = use;
            } else {
                if (ope2ope.count(use))
                    src = ope2ope[use];
                else {
                    src = getTempOperand(use);
                    ope2ope[use] = src;
                }
            }
            newPhi->addSrc(block2block[it.first], src);
        }
        auto def = oldPhi->getDef();
        Operand* dst;
        if (ope2ope.count(def))
            dst = ope2ope[def];
        else {
            dst = getTempOperand(def);
            ope2ope[def] = dst;
        }
        newPhi->setDef(dst);
    }
    for (auto block : retBlocks) {
        block->addSucc(exit);
        exit->addPred(block);
    }
    new UncondBrInstruction(entry, parent);
    parent->addSucc(entry);
    entry->addPred(parent);
    in->getParent()->remove(in);
    if (ret) {
        Instruction* newIn;
        int size = retOpes.size();
        if (size > 1) {
            PhiInstruction* phi = new PhiInstruction(ret);
            for (int i = 0; i < size; i++)
                phi->addSrc(retBlocks[i], retOpes[i]);
            newIn = phi;
        } else {
            auto zero =
                new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
            newIn = new BinaryInstruction(BinaryInstruction::ADD, ret,
                                          retOpes[0], zero);
        }
        newIn->setParent(exit);
        exit->insertFront(newIn);
    }
}

void AutoInline::calRecursion() {
    // sysy 不存在循环递归 那就没必要单独判断了
    // for (auto it = unit->begin(); it != unit->end(); it++) {
    //     stack<Function*> stk;
    //     set<Function*> visited;
    //     stk.push(*it);
    //     while (!stk.empty()) {
    //         auto func = stk.top();
    //         stk.pop();
    //         if (func == *it && visited.count(func)) {
    //             func->setRecur();
    //             break;
    //         }
    //         if (visited.count(func))
    //             continue;
    //         visited.insert(func);
    //         auto preds = func->getPreds();
    //         for (auto it : preds)
    //             stk.push(it.first);
    //     }
    // }
}

void AutoInline::calInstNum() {
    for (auto it = unit->begin(); it != unit->end(); it++) {
        int num = 0;
        auto func = *it;
        for (auto block : func->getBlockList())
            for (auto in = block->begin(); in != block->end();
                 in = in->getNext())
                if (!in->isAlloc() && !in->isUncond() && !in->isCond())
                    num++;
        func->setInstNum(num);
    }
}