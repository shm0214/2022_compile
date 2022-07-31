#include "InsReorder.h"
#include <map>
#include <set>
#include <vector>

using namespace std;

// 移动指令顺序，使得use离def足够的近，从而减少变量的生存期
// 从而减少溢出的可能

void InsReorder::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void InsReorder::pass(Function* func) {
    for (auto block : func->getBlockList())
        pass(block);
}

void InsReorder::pass(BasicBlock* block) {
    auto head = block->begin()->getPrev();
    vector<Instruction*> ins;
    set<Operand*> defs;
    vector<InsNode*> starts;
    map<Instruction*, InsNode*> in2node;
    for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
        auto node = new InsNode(in);
        in2node[in] = node;
        auto def = in->getDef();
        if (def)
            defs.insert(def);
        ins.push_back(in);
    }
    // 原始指令顺序遍历保证指令所用的use一定def了
    // 建图
    for (auto in : ins) {
        auto node = in2node[in];
        set<Operand*> temp;
        for (auto use : in->getUse()) {
            if (defs.count(use))
                temp.insert(use);
        }
        auto flag = false;
        if (in->isCall()) {
            IdentifierSymbolEntry* funcSE =
                (IdentifierSymbolEntry*)(((CallInstruction*)in)->getFuncSE());
            if (funcSE->getName() != "llvm.memset.p0.i32") {
                if (funcSE->isSysy()) {
                    flag = true;
                } else {
                    auto func = funcSE->getFunction();
                    if (func->getEssential() == 1)
                        flag = true;
                }
            }
            if (flag) {
                starts.push_back(node);
                continue;
            }
        }
        if (temp.empty() || in->isPhi()) {
            if (in->isCall() && in->getPrev()->getInstType() != -1) {
                auto prev = in->getPrev();
                in2node[prev]->addChild(node);
            } else
                starts.push_back(node);
            // if (in->isAlloc() || in->isRet())
            //     starts.push_back(node);
            // else if (in->isCall()) {
            //     auto prev = in->getPrev();
            //     in2node[prev]->addChild(node);
            // } else
            //     assert(0);
        } else
            for (auto o : temp)
                in2node[o->getDef()]->addChild(node);
    }
    head->setNext(head);
    head->setPrev(head);
    // 遍历
    for (auto start : starts) {
        dfs(start, block);
        // block->output();
    }
    // 改一下alloc的位置，因为不知道会不会对别的优化造成影响
    // 以及调整phi的位置
    for (auto it = starts.rbegin(); it != starts.rend(); it++) {
        auto in = (*it)->in;
        if (block == block->getParent()->getEntry()) {
            if (in->isAlloc()) {
                block->remove(in);
                block->insertFront(in);
            }
        } else if (in->isPhi()) {
            block->remove(in);
            block->insertFront(in);
        }
    }
    for (auto in = block->rbegin(); in != block->rend(); in = in->getPrev())
        if (in->isRet() || in->isCond() || in->isUncond()) {
            block->remove(in);
            block->insertBack(in);
            break;
        }
}

void InsReorder::dfs(InsNode* node, BasicBlock* block) {
    node->visited = true;
    block->insertBack(node->in);
    for (auto child : node->children) {
        bool flag = true;
        for (auto parent : child->parents)
            if (!parent->visited) {
                flag = false;
                break;
            }
        if (!flag)
            continue;
        dfs(child, block);
    }
}
