// #include "TreeHeightBalance.h"
// #include <queue>
// #include "LVAForIR.h"

// // ref: engineering a compiler 8.4.2

// using namespace std;

// int getPrecedence(Instruction* in) {
//     assert(in->isBin());
//     int op = in->getOpcode();
//     switch (op) {
//         case BinaryInstruction::ADD:
//             return 3;
//         case BinaryInstruction::MUL:
//             return 4;
//         case BinaryInstruction::AND:
//             return 2;
//         case BinaryInstruction::OR:
//             return 1;
//     }
//     return -1;
// }

// void TreeHeightBalance::pass() {
//     auto iter = unit->begin();
//     while (iter != unit->end())
//         pass(*iter++);
// }

// void TreeHeightBalance::pass(Function* func) {
//     lva.pass(func);
//     // 由于参数要copy一次 所以要单独统计copy的def
//     if (!params.empty())
//         params.clear();
//     auto entry = func->getEntry();
//     for (auto in = entry->begin(); in != entry->end(); in = in->getNext())
//         if (in->isBin() && in->getOpcode() == BinaryInstruction::ADD) {
//             auto def = in->getDef();
//             auto uses = in->getUse();
//             if (uses[0]->getEntry()->isVariable() && uses[1]->isZero())
//                 params.insert(def);
//         }
//     for (auto block : func->getBlockList())
//         pass(block);
// }

// void TreeHeightBalance::pass(BasicBlock* block) {
//     priority_queue<PAIR, vector<PAIR>, cmp> roots;
//     rank.clear();
//     rootSet.clear();
//     delList.clear();
//     for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
//         auto def = in->getDef();
//         if (def)
//             rank[def] = -1;
//         // 可交换 可结合
//         // 加法 乘法 and or
//         // xor 先不考虑了
//         if (in->isComAndAsso()) {
//             auto useNum = 0;
//             Instruction* use = nullptr;
//             for (auto it = def->use_begin(); it != def->use_end(); it++)
//                 if ((*it)->getParent() == block) {
//                     useNum++;
//                     use = *it;
//                 }
//             auto flag = false;
//             if (useNum > 1)
//                 flag = true;
//             if (useNum == 1)
//                 if (!in->hasEqualOp(use))
//                     flag = true;
//             if (params.count(def))
//                 flag = false;
//             if (flag) {
//                 rootSet.insert(def);
//                 roots.push({def, getPrecedence(in)});
//             }
//         }
//     }
//     for (auto ope : lva.getLiveOut(block)) {
//         if (ope->isConst())
//             continue;
//         auto def = ope->getDef();
//         if (!rootSet.count(ope) && def && def->isComAndAsso()) {
//             rootSet.insert(ope);
//             roots.push({ope, getPrecedence(def)});
//         }
//     }
//     while (!roots.empty()) {
//         auto var = roots.top().first;
//         roots.pop();
//         balance(var);
//     }
//     while (delList.size()) {
//         auto in = *(delList.begin());
//         block->remove(in);
//         delList.erase(in);
//     }
// }

// void TreeHeightBalance::balance(Operand* root) {
//     if (rank[root] >= 0)
//         return;
//     auto in = root->getDef();
//     auto uses = in->getUse();
//     priority_queue<PAIR, vector<PAIR>, cmp> q;
//     auto def0 = uses[0]->getDef();
//     auto def1 = uses[1]->getDef();
//     if (def0 && def0->isComAndAsso() && def1 && def1->isComAndAsso()) {
//         delList.insert(in);
//         rank[root] = flatten(uses[0], q) + flatten(uses[1], q);
//         // int old = delList.size();
//         rebuild(root, q, in);
//         // if ((int)delList.size() == old)
//         // delList.erase(in);
//     }
//     while (!q.empty())
//         q.pop();
// }

// int TreeHeightBalance::flatten(Operand* var,
//                                priority_queue<PAIR, vector<PAIR>, cmp>& q) {
//     if (var->isConst()) {
//         rank[var] = 0;
//         q.push({var, rank[var]});
//     } else {
//         auto defIn = var->getDef();
//         // if (!defIn->isComAndAsso())
//         //     return -1;
//         if (!params.count(var) && defIn) {
//             bool flag = true;
//             for (auto it = var->use_begin(); it != var->use_end(); it++) {
//                 if (!delList.count(*it)) {
//                     flag = false;
//                     break;
//                 } else if (!defIn->hasEqualOp(*it)) {
//                     flag = false;
//                     break;
//                 }
//             }
//             if (flag)
//                 delList.insert(defIn);
//         }
//         auto block = var->getDef()->getParent();
//         auto UEVar = lva.getUEVar(block);
//         if (UEVar.count(var)) {
//             rank[var] = 1;
//             q.push({var, rank[var]});
//         } else if (params.count(var)) {
//             rank[var] = 1;
//             q.push({var, rank[var]});
//         } else {
//             if (rootSet.count(var)) {
//                 balance(var);
//                 q.push({var, rank[var]});
//             } else {
//                 auto in = var->getDef();
//                 auto uses = in->getUse();
//                 auto def0 = uses[0]->getDef();
//                 auto def1 = uses[1]->getDef();
//                 if (def0) {
//                     if (def0->isComAndAsso())
//                         flatten(uses[0], q);
//                     else if (def0->isPhi() ||
//                              (def0->isBin() &&
//                               def0->getOpcode() == BinaryInstruction::SUB)) {
//                         rank[uses[0]] = 1;
//                         q.push({uses[0], 1});
//                     }
//                 }
//                 if (def1) {
//                     if (def1->isComAndAsso())
//                         flatten(uses[1], q);
//                     else if (def1->isPhi() ||
//                              (def1->isBin() &&
//                               def1->getOpcode() == BinaryInstruction::SUB)) {
//                         rank[uses[1]] = 1;
//                         q.push({uses[1], 1});
//                     }
//                 }
//             }
//         }
//     }
//     return rank[var];
// }

// void TreeHeightBalance::rebuild(Operand* root,
//                                 priority_queue<PAIR, vector<PAIR>, cmp>& q,
//                                 Instruction* in) {
//     while (!q.empty()) {
//         auto left = q.top().first;
//         q.pop();
//         auto right = q.top().first;
//         q.pop();
//         if (left->isConst() && right->isConst()) {
//             auto val1 = left->getConstVal();
//             auto val2 = right->getConstVal();
//             double val;
//             int op = in->getOpcode();
//             switch (op) {
//                 case BinaryInstruction::ADD:
//                     val = val1 + val2;
//                     break;
//                 case BinaryInstruction::MUL:
//                     val = val1 * val2;
//                     break;
//                 case BinaryInstruction::AND:
//                     val = val1 && val2;
//                     break;
//                 case BinaryInstruction::OR:
//                     val = val1 || val2;
//                     break;
//             }
//             if (q.empty()) {
//                 root->setEntry(
//                     new ConstantSymbolEntry(in->getDef()->getType(), val));
//                 rank[root] = 0;
//             } else {
//                 Operand* new_ = new Operand(
//                     new ConstantSymbolEntry(in->getDef()->getType(), val));
//                 rank[new_] = 0;
//                 q.push({new_, 0});
//             }
//         } else {
//             Operand* newDef;
//             if (q.empty())
//                 newDef = root;
//             else
//                 newDef = new Operand(new TemporarySymbolEntry(
//                     TypeSystem::intType, SymbolTable::getLabel()));
//             auto block = in->getParent();
//             assert(in->isBin());
//             auto op = in->getOpcode();
//             auto newIn = new BinaryInstruction(op, newDef, left, right);
//             newIn->setParent(block);
//             block->insertBefore(newIn, in);
//             rank[newDef] = rank[left] + rank[right];
//             if (!q.empty())
//                 q.push({newDef, rank[newDef]});
//         }
//     }
// }

#include "TreeHeightBalance.h"

using namespace std;

// 参考那个书效果太差了 问题多时间慢
// 简单粗暴处理 只针对连加/减 转换成乘法再加/减
// 针对样例： hoist instruction-combine

void TreeHeightBalance::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void TreeHeightBalance::pass(Function* func) {
    for (auto block : func->getBlockList())
        pass(block);
}

void TreeHeightBalance::pass(BasicBlock* block) {
    map<Operand*, int, cmp> operands;
    vector<Instruction*> addIns;
    Operand* lastDef = nullptr;
    for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
        bool flag = false;
        if (in->isAdd() || in->isSub()) {
            // copy param
            if (in->getUse()[0]->getEntry()->isVariable())
                continue;
            if (!in->getDef()->getType()->isInt())
                continue;
            auto next = in->getNext();
            auto def = in->getDef();
            if (!lastDef) {
                if (def->usersNum() != 1)
                    continue;
                // 保证不会只有一条指令
                if ((next->isAdd() || next->isSub()) &&
                    next->getUse()[0] == def) {
                    lastDef = def;
                    auto uses = in->getUse();
                    operands[uses[0]]++;
                    if (in->isAdd())
                        operands[uses[1]]++;
                    else
                        operands[uses[1]]--;
                    addIns.push_back(in);
                }
            } else {
                auto uses = in->getUse();
                if (uses[0] == lastDef && def->usersNum() == 1) {
                    addIns.push_back(in);
                    lastDef = def;
                    if (in->isAdd())
                        operands[uses[1]]++;
                    else
                        operands[uses[1]]--;
                } else
                    flag = true;
            }
        } else if (lastDef)
            flag = true;
        if (flag) {
            convert(operands, addIns);
            operands.clear();
            addIns.clear();
            lastDef = nullptr;
        }
    }
}

void TreeHeightBalance::convert(map<Operand*, int, cmp> operands,
                                vector<Instruction*> addIns) {
    // 先不管浮点了
    auto lastIn = *(addIns.rbegin());
    auto oldRes = lastIn->getDef();
    auto type = oldRes->getType();
    vector<Instruction*> newIns;
    vector<Operand*> addDsts;
    vector<Operand*> subDsts;
    auto block = addIns[0]->getParent();
    double val = 0;
    for (auto it : operands) {
        if (it.first->isConst())
            val += it.first->getConstVal() * it.second;
        else if (it.second != 1 && it.second != -1) {
            auto src1 = it.first;
            auto num = abs(it.second);
            auto src2 =
                new Operand(new ConstantSymbolEntry(TypeSystem::intType, num));
            if (type->isFloat()) {
                auto sitofpDst = new Operand(
                    new TemporarySymbolEntry(type, SymbolTable::getLabel()));
                auto sitofp = new SitofpInstruction(sitofpDst, src2);
                sitofp->setParent(block);
                newIns.push_back(sitofp);
                src2 = sitofpDst;
            }
            auto dst = new Operand(
                new TemporarySymbolEntry(type, SymbolTable::getLabel()));
            auto in =
                new BinaryInstruction(BinaryInstruction::MUL, dst, src1, src2);
            in->setParent(block);
            newIns.push_back(in);
            if (it.second > 0)
                addDsts.push_back(dst);
            else
                subDsts.push_back(dst);
        } else if (it.second == 1)
            addDsts.push_back(it.first);
        else if (it.second == -1)
            subDsts.push_back(it.first);
    }
    if (val) {
        auto imm = new Operand(new ConstantSymbolEntry(type, val));
        addDsts.push_back(imm);
    }
    Operand* res;
    if (!addDsts.empty()) {
        if (addDsts.size() == 1) {
            // assert(dsts[0]->isConst());
            res = addDsts[0];
        } else {
            res = new Operand(
                new TemporarySymbolEntry(type, SymbolTable::getLabel()));
            auto in = new BinaryInstruction(BinaryInstruction::ADD, res,
                                            addDsts[0], addDsts[1]);
            in->setParent(block);
            newIns.push_back(in);
            for (auto it = addDsts.begin() + 2; it != addDsts.end(); it++) {
                auto newRes = new Operand(
                    new TemporarySymbolEntry(type, SymbolTable::getLabel()));
                in = new BinaryInstruction(BinaryInstruction::ADD, newRes, res,
                                           *it);
                in->setParent(block);
                newIns.push_back(in);
                res = newRes;
            }
        }
    } else {
        res = new Operand(new ConstantSymbolEntry(type, 0));
    }
    if (!subDsts.empty()) {
        for (auto it = subDsts.begin(); it != subDsts.end(); it++) {
            auto newRes = new Operand(
                new TemporarySymbolEntry(type, SymbolTable::getLabel()));
            auto in =
                new BinaryInstruction(BinaryInstruction::SUB, newRes, res, *it);
            in->setParent(block);
            newIns.push_back(in);
            res = newRes;
        }
    }

    while (oldRes->usersNum())
        (*(oldRes->use_begin()))->replaceUse(oldRes, res);
    auto nextIn = lastIn->getNext();
    auto prevIn = addIns[0]->getPrev();
    for (auto in : newIns) {
        prevIn->setNext(in);
        in->setPrev(prevIn);
        prevIn = in;
    }
    if (newIns.size()) {
        auto newLastIn = *(newIns.rbegin());
        newLastIn->setNext(nextIn);
        nextIn->setPrev(newLastIn);
    } else {
        nextIn->setPrev(prevIn);
        prevIn->setNext(nextIn);
    }
    // for (auto in : addIns)
    //     delete in;
}
