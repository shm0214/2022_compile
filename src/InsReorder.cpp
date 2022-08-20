#include "InsReorder.h"
#include <map>
#include <set>
#include <vector>

using namespace std;

// 移动指令顺序，使得use离def足够的近，从而减少变量的生存期
// 从而减少溢出的可能

// 只针对参数数量>=10的函数和没有call的块
// 针对样例： hoist instruction-combine

void InsReorder::pass() {
    auto iter = unit->begin();
    while (iter != unit->end()) {
        auto type = (FunctionType*)((*iter)->getSymPtr()->getType());
        auto paramsType = type->getParamsType();
        if (paramsType.size() >= 10) {
            auto flag = true;
            for (auto t : paramsType)
                if (t->isArray() || t->isFloat()) {
                    flag = false;
                    break;
                }
            if (flag)
                pass(*iter);
        }
        iter++;
    }
}

void InsReorder::pass(Function* func) {
    for (auto block : func->getBlockList())
        pass(block);
}

void InsReorder::pass(BasicBlock* block) {
    // 改回以前线性插入吧，用图速度太慢了
    auto head = block->begin()->getPrev();
    vector<Instruction*> ins;
    map<Instruction*, int> in2no;
    int no = 0;
    set<Operand*> defs;
    for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
        if (in->isCall())
            return;
        auto def = in->getDef();
        if (def)
            defs.insert(def);
        ins.push_back(in);
        in2no[in] = no++;
    }
    // 原始指令顺序遍历保证指令所用的use一定def了
    head->setNext(head);
    head->setPrev(head);
    auto noUseIn = head;
    for (auto in : ins) {
        // block->output();
        set<Operand*> temp;
        for (auto u : in->getUse())
            if (defs.count(u))
                temp.insert(u);
        if (temp.empty()) {
            block->insertAfter(in, noUseIn);
            noUseIn = in;
        } else {
            for (auto in1 = block->rbegin(); in1 != block->rend();
                 in1 = in1->getPrev()) {
                if (temp.count(in1->getDef())) {
                    block->insertAfter(in, in1);
                    break;
                }
            }
        }
    }
    // lastUseIn->setNext(head);
    // head->setPrev(lastUseIn);

    for (auto in = block->rbegin(); in != block->rend(); in = in->getPrev())
        if (in->isRet() || in->isCond() || in->isUncond()) {
            block->remove(in);
            block->insertBack(in);
            break;
        }
}

// void InsReorder::pass(BasicBlock* block) {
//     auto head = block->begin()->getPrev();
//     vector<Instruction*> ins;
//     set<Operand*> defs;
//     vector<InsNode*> starts;
//     map<Instruction*, InsNode*> in2node;
//     for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
//         if (in->isCall())
//             return;
//         auto def = in->getDef();
//         if (def)
//             defs.insert(def);
//         ins.push_back(in);
//     }
//     // 原始指令顺序遍历保证指令所用的use一定def了
//     // 建图
//     for (auto in : ins) {
//         auto node = new InsNode(in);
//         in2node[in] = node;
//         set<Operand*> temp;
//         for (auto use : in->getUse()) {
//             if (defs.count(use))
//                 temp.insert(use);
//         }
//         if (temp.empty() || in->isPhi()) {
//             starts.push_back(node);
//         } else
//             for (auto o : temp)
//                 in2node[o->getDef()]->addChild(node);
//     }
//     head->setNext(head);
//     head->setPrev(head);
//     // 遍历
//     for (auto start : starts) {
//         dfs(start, block);
//         block->output();
//     }
//     // 改一下alloc的位置，因为不知道会不会对别的优化造成影响
//     // 以及调整phi的位置
//     for (auto it = starts.rbegin(); it != starts.rend(); it++) {
//         auto in = (*it)->in;
//         if (block == block->getParent()->getEntry()) {
//             if (in->isAlloc()) {
//                 block->remove(in);
//                 block->insertFront(in);
//             }
//         } else if (in->isPhi()) {
//             block->remove(in);
//             block->insertFront(in);
//         }
//     }
//     for (auto in = block->rbegin(); in != block->rend(); in = in->getPrev())
//         if (in->isRet() || in->isCond() || in->isUncond()) {
//             block->remove(in);
//             block->insertBack(in);
//             break;
//         }
// }

// void InsReorder::dfs(InsNode* node, BasicBlock* block) {
//     node->visited = true;
//     block->insertBack(node->in);
//     for (auto child : node->children) {
//         bool flag = true;
//         for (auto parent : child->parents)
//             if (!parent->visited) {
//                 flag = false;
//                 break;
//             }
//         if (!flag)
//             continue;
//         dfs(child, block);
//     }
// }
