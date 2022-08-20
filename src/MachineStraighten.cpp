#include "MachineStraighten.h"
#include <algorithm>

void MachineStraighten::pass() {
    for (auto func : unit->getFuncs()) {
        bool again = true;
        while (again) {
            again = false;
            again = pass4(func) || again;
            again = pass1(func) || again;
            again = pass2(func) || again;
            again = pass3(func) || again;
        }
    }
}

bool MachineStraighten::pass1(MachineFunction* func) {
    // 合并单后继与单前驱块
    bool flag = false;
    for (auto block : func->getBlocks()) {
        auto succs = block->getSuccs();
        if (succs.size() == 1 && succs[0]->getPreds().size() == 1) {
            assert(succs[0]->getPreds()[0] == block);
            auto succ = succs[0];
            if (succ == func->getEntry())
                continue;
            flag = true;
            fuseBlock(func, block, succ);
            pass1(func);
        }
    }
    return flag;
}

void MachineStraighten::fuseBlock(MachineFunction* func,
                                  MachineBlock* i,
                                  MachineBlock* j) {
    // 最后一条指令为无条件跳转/顺序连接
    auto tail = *(i->getInsts().rbegin());
    if (tail->isB())
        i->remove(tail);
    for (auto in : j->getInsts())
        i->InsertInst(in);
    tail = *(i->getInsts().rbegin());
    if (!tail->isB()) {
        // 不以b结尾 要么顺序连接单一后继 要么跳出无后继
        auto temp = j->getSuccs().size();
        assert(temp == 1 || temp == 0);
        if (temp == 1) {
            auto block = j->getSuccs()[0];
            auto label = new MachineOperand(block->getLabel());
            i->InsertInst(
                new BranchMInstruction(i, BranchMInstruction::B, label));
        }
    }
    i->cleanSucc();
    for (auto block : j->getSuccs()) {
        i->addSucc(block);
        block->removePred(j);
        block->addPred(i);
    }
    func->removeBlock(j);
}

bool MachineStraighten::pass2(MachineFunction* func) {
    // 删除只有一句无条件跳转的基本块
    bool flag = false;
    auto& blocklist = func->getBlocks();
    for (auto it = blocklist.begin(); it != blocklist.end();) {
        auto block = *it;
        auto ins = block->getInsts();
        if (ins.size() == 1 && ins[0]->isB()) {
            auto use = ins[0]->getUse()[0];
            assert(use->isLabel());
            std::string label = use->getLabel();
            label = label.substr(2);
            int no = std::stoi(label);
            auto next = func->getBlock(no);
            if (block == func->getEntry()) {
                it++;
                continue;
            }
            next->removePred(block);
            for (auto pred : block->getPreds()) {
                auto succs = pred->getSuccs();
                int num = succs.size();
                auto ins = pred->getInsts();
                int tmp = 0;
                for (auto it = ins.rbegin(); it != ins.rend(); it++) {
                    if (!(*it)->isB())
                        break;
                    tmp++;
                    auto u = (*it)->getUse()[0];
                    assert(u->isLabel());
                    int no_ = std::stoi(u->getLabel().substr(2));
                    if (no_ == block->getNo()) {
                        (*it)->replaceUse(u,
                                          new MachineOperand(next->getLabel()));
                        tmp = num;
                    }
                }
                if (tmp < num) {
                    assert(tmp == num - 1);
                    auto b = func->getNext(pred);
                    assert(b == block);
                    pred->InsertInst(new BranchMInstruction(
                        pred, BranchMInstruction::B,
                        new MachineOperand(next->getLabel())));
                }
                next->addPred(pred);
                pred->addSucc(next);
                pred->removeSucc(block);
            }
            it = blocklist.erase(it);
            flag = true;
        } else
            it++;
    }
    return flag;
}

bool MachineStraighten::pass3(MachineFunction* func) {
    // 删除顺序连接的基本块的无条件跳转
    bool flag = false;
    auto blocks = func->getBlocks();
    auto it1 = blocks.begin();
    auto it2 = blocks.begin() + 1;
    while (it2 != blocks.end()) {
        auto succs = (*it1)->getSuccs();
        if (std::find(succs.begin(), succs.end(), *it2) == succs.end()) {
            it1++;
            it2++;
            continue;
        }
        auto ins = (*it1)->getInsts();
        if (*it1 == func->getEntry() || ins.size() == 0) {
            it1++;
            it2++;
            continue;
        }
        auto in1 = *(ins.rbegin());
        if (!in1->isB()) {
            it1++;
            it2++;
            continue;
        }
        auto use1 = in1->getUse()[0];
        assert(use1->isLabel());
        std::string label1 = use1->getLabel();
        label1 = label1.substr(2);
        int no1 = std::stoi(label1);
        auto block1 = func->getBlock(no1);
        if (ins.size() > 1) {
            auto in2 = *(ins.rbegin() + 1);
            if (in2->isB()) {
                assert(in2->getCond() != MachineInstruction::NONE);
                assert(in1->getCond() == MachineInstruction::NONE);
                if (block1 == *it2) {
                    (*it1)->remove(in1);
                    flag = true;
                } else {
                    auto use2 = in2->getUse()[0];
                    assert(use2->isLabel());
                    std::string label2 = use2->getLabel();
                    label2 = label2.substr(2);
                    int no2 = std::stoi(label2);
                    auto block2 = func->getBlock(no2);
                    if (block2 == *it2) {
                        auto cond = in2->getCond();
                        if (cond <= 1)
                            cond = 1 - cond;
                        else
                            cond = 7 - cond;
                        in2->setCond(cond);
                        in2->replaceUse(use2, use1);
                        (*it1)->remove(in1);
                        flag = true;
                    } else
                        // shouldn't reach here
                        assert(0);
                }
            } else {
                if (block1 == *it2) {
                    (*it1)->remove(in1);
                    flag = true;
                }
            }
        } else {
            if (block1 == *it2) {
                (*it1)->remove(in1);
                flag = true;
            }
        }
        it1++;
        it2++;
    }
    return flag;
}

bool MachineStraighten::pass4(MachineFunction* func) {
    // 处理空块的跳转
    bool flag = false;
    std::vector<MachineBlock*> temp;
    for (auto block : func->getBlocks()) {
        if (block->getSize() == 0) {
            temp.push_back(block);
            auto next = func->getNext(block);
            if (!next)
                continue;
            auto preds = block->getPreds();
            for (auto pred : preds) {
                auto ins = pred->getInsts();
                for (auto it = ins.rbegin(); it != ins.rend(); it++) {
                    if (!(*it)->isB()) {
                        pred->removeSucc(block);
                        next->addPred(pred);
                        pred->addSucc(next);
                        break;
                    }
                    auto use = (*it)->getUse()[0];
                    assert(use->isLabel());
                    std::string label = use->getLabel();
                    label = label.substr(2);
                    int no = std::stoi(label);
                    auto b = func->getBlock(no);
                    if (b == block) {
                        auto l = new MachineOperand(next->getLabel());
                        (*it)->replaceUse(use, l);
                        pred->removeSucc(block);
                        next->addPred(pred);
                        pred->addSucc(next);
                        break;
                    }
                }
            }
            next->removePred(block);
        }
    }
    if (temp.size())
        flag = true;
    for (auto b : temp) {
        func->removeBlock(b);
    }
    return flag;
}