#include "Function.h"
#include <list>
#include <queue>
#include <vector>
#include "Type.h"
#include "Unit.h"

using namespace std;

extern FILE* yyout;

Function::Function(Unit* u, SymbolEntry* s) {
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
    ((IdentifierSymbolEntry*)s)->setFunction(this);
    call = false;
    recur = false;
    u->insertFunc(this);
}

int TreeNode::Num = 0;

Function::~Function() {
    // auto delete_list = block_list;
    // for (auto &i : delete_list)
    //     delete i;
    // parent->removeFunc(this);
}

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock* bb) {
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::dfs1(BasicBlock* block, std::set<BasicBlock*>& v) {
    v.insert(block);
    for (auto it = block->succ_begin(); it != block->succ_end(); it++) {
        if (v.find(*it) == v.end()) {
            (*it)->output();
            dfs1(*it, v);
        }
    }
}

void Function::output() {
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type* retType = funcType->getRetType();
    std::vector<SymbolEntry*> paramsSe = funcType->getParamsSe();
    if (!paramsSe.size())
        fprintf(yyout, "define %s %s() {\n", retType->toStr().c_str(),
                sym_ptr->toStr().c_str());
    else {
        fprintf(yyout, "define %s %s(", retType->toStr().c_str(),
                sym_ptr->toStr().c_str());
        for (long unsigned int i = 0; i < paramsSe.size(); i++) {
            if (i)
                fprintf(yyout, ", ");
            fprintf(yyout, "%s %s", paramsSe[i]->getType()->toStr().c_str(),
                    paramsSe[i]->toStr().c_str());
        }
        fprintf(yyout, ") {\n");
    }
    std::set<BasicBlock*> v;
    entry->output();
    dfs1(entry, v);
    fprintf(yyout, "}\n");
    // fflush(yyout);
}

void Function::dfs(AsmBuilder* builder,
                   BasicBlock* block,
                   std::set<BasicBlock*>& v,
                   std::map<BasicBlock*, MachineBlock*>& map) {
    v.insert(block);
    for (auto it = block->succ_begin(); it != block->succ_end(); it++) {
        if (v.find(*it) == v.end()) {
            (*it)->genMachineCode(builder);
            map[*it] = builder->getBlock();
            dfs(builder, *it, v, map);
        }
    }
}

void Function::genMachineCode(AsmBuilder* builder) {
    auto cur_unit = builder->getUnit();
    auto cur_func = new MachineFunction(cur_unit, this->sym_ptr);
    builder->setFunction(cur_func);
    std::map<BasicBlock*, MachineBlock*> map;
    // for (auto block : block_list) {
    //     block->genMachineCode(builder);
    //     map[block] = builder->getBlock();
    // }
    std::set<BasicBlock*> v;
    entry->genMachineCode(builder);
    map[entry] = builder->getBlock();
    dfs(builder, entry, v, map);

    // while (!q.empty()) {
    //     auto bb = q.top();
    //     bb->genMachineCode(builder);
    //     map[bb] = builder->getBlock();
    //     for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++) {
    //         if (v.find(*succ) == v.end()) {
    //             v.insert(*succ);
    //             q.push(*succ);
    //         }
    //     }
    //     q.pop();
    // }

    // Add pred and succ for every block
    for (auto block : block_list) {
        auto mblock = map[block];
        for (auto pred = block->pred_begin(); pred != block->pred_end(); pred++)
            mblock->addPred(map[*pred]);
        for (auto succ = block->succ_begin(); succ != block->succ_end(); succ++)
            mblock->addSucc(map[*succ]);
    }
    cur_func->setEntry(map[entry]);
    cur_unit->InsertFunc(cur_func);
}

vector<vector<int>> Function::getBlockMap() {
    int len = block_list.size();
    vector<vector<int>> m(len, vector<int>(len, 0));
    for (int i = 0; i < len; i++) {
        auto iter = block_list[i]->succ_begin();
        auto end = block_list[i]->succ_end();
        while (iter != end) {
            int j = find(block_list.begin(), block_list.end(), *iter) -
                    block_list.begin();
            m[i][j] = 1;
            iter++;
        }
    }
    return m;
}

void Function::computeDFSTree() {
    TreeNode::Num = 0;
    int len = block_list.size();
    preOrder2DFS.resize(len);
    bool* visited = new bool[len]{};
    DFSTreeRoot = new TreeNode(entry);
    preOrder2DFS[DFSTreeRoot->num] = DFSTreeRoot;
    search(DFSTreeRoot, visited);
    delete[] visited;
}

void Function::search(TreeNode* node, bool* visited) {
    int n = getIndex(node->block);
    visited[n] = true;
    auto block = block_list[n];
    for (auto it = block->succ_begin(); it != block->succ_end(); it++) {
        int idx = getIndex(*it);
        if (!visited[idx]) {
            TreeNode* child = new TreeNode(*it);
            preOrder2DFS[child->num] = child;
            child->parent = node;
            node->addChild(child);
            search(child, visited);
        }
    }
}

int Function::eval(int v, int* ancestors) {
    int a = ancestors[v];
    while (a != -1 && ancestors[a] != -1) {
        if (sdoms[v] > sdoms[a])
            v = a;
        a = ancestors[a];
    }
    return v;
}

void Function::computeSdom() {
    int len = block_list.size();
    sdoms.resize(len);
    // 应该有问题。。。。
    // for (auto it = preOrder2DFS.rbegin(); it != preOrder2DFS.rend(); it++) {
    //     auto block = (*it)->block;
    //     int s = block->order;
    //     for (auto it1 = block->pred_begin(); it1 != block->pred_end(); it1++) {
    //         int order = (*it1)->order;
    //         s = min(s, (*it1)->order);
    //         if (order > block->order) {
    //             int sdom = sdoms[order];
    //             while (sdom >= block->order)
    //                 sdom = sdoms[sdom];
    //             if (sdom < block->order)
    //                 s = min(s, sdom);
    //         }
    //     }
    //     sdoms[block->order] = s;
    // }
    int* ancestors = new int[len];
    for (int i = 0; i < len; i++) {
        sdoms[i] = i;
        ancestors[i] = -1;
    }
    for (auto it = preOrder2DFS.rbegin(); (*it)->block != entry; it++) {
        auto block = (*it)->block;
        int s = block->order;
        for (auto it1 = block->pred_begin(); it1 != block->pred_end(); it1++) {
            int z = eval((*it1)->order, ancestors);
            if (sdoms[z] < sdoms[s])
                sdoms[s] = sdoms[z];
        }
        ancestors[s] = (*it)->parent->num;
    }
    delete[] ancestors;
    // for (auto c : sdoms)
    //     cout << c << " ";
    // cout << endl;
}

int Function::LCA(int i, int j) {
    TreeNode* n1 = preOrder2dom[i];
    TreeNode* n2 = preOrder2dom[j];
    int h1 = n1->getHeight();
    int h2 = n2->getHeight();
    if (h1 > h2) {
        swap(h1, h2);
        swap(n1, n2);
    }
    int h = h2 - h1;
    for (int i = 0; i < h; i++)
        n2 = n2->parent;
    while (n1 && n2) {
        if (n1 == n2)
            return n1->num;
        n1 = n1->parent;
        n2 = n2->parent;
    }
    // shouldn't reach here
    return -1;
}

void Function::computeIdom() {
    int len = block_list.size();
    idoms.resize(len);
    domTreeRoot = new TreeNode(entry, 0);
    preOrder2dom.resize(len);
    preOrder2dom[entry->order] = domTreeRoot;
    idoms[entry->order] = 0;
    for (auto it = preOrder2DFS.begin() + 1; it != preOrder2DFS.end(); it++) {
        int p = LCA((*it)->parent->num, sdoms[(*it)->num]);
        idoms[(*it)->num] = p;
        auto parent = preOrder2dom[p];
        TreeNode* node = new TreeNode((*it)->block, 0);
        node->parent = parent;
        parent->addChild(node);
        preOrder2dom[(*it)->num] = node;
    }
    // for (auto c : idoms)
    //     cout << c << " ";
    // cout << endl;
    // for (int i = 0; i < (int)idoms.size(); i++)
    //     cout << preOrder2DFS[i]->block->getNo()<< ":"
    //          << preOrder2DFS[idoms[i]]->block->getNo()<< " ";
    // cout << endl;
}

void Function::computeDomFrontier() {
    for (auto block : block_list) {
        if (block->getNumOfPred() >= 2) {
            for (auto it = block->pred_begin(); it != block->pred_end(); it++) {
                int runner = (*it)->order;
                while (runner != idoms[block->order]) {
                    preOrder2DFS[runner]->block->domFrontier.insert(block);
                    runner = idoms[runner];
                }
            }
        }
    }
    // for (auto block : block_list) {
    //     cout << preOrder2DFS[block->order]->block->getNo() << ": ";
    //     for (auto c : block->domFrontier) {
    //         cout << preOrder2DFS[c->order]->block->getNo() << " ";
    //     }
    //     cout << endl;
    // }
}

void Function::domTest() {
#define a2b(a, b)                          \
    block_list[a]->addSucc(block_list[b]); \
    block_list[b]->addPred(block_list[a])
    // https://blog.csdn.net/dashuniuniu/article/details/103462147
    // for (int i = 0; i < 25; i++)
    //     new BasicBlock(this);
    // entry = block_list[0];
    // int links[25][3] = {{2, 14},      {3, 21}, {4, 11, 18},  {5},      {6, 9},
    //                     {7},          {1, 8},  {6},          {7, 10},  {7},
    //                     {12, 14, 15}, {9, 13}, {},           {15},     {16},
    //                     {17},         {8, 15}, {19, 21, 24}, {15, 20}, {},
    //                     {20, 22, 24}, {23},    {16},         {25},     {}};
    // for (int i = 0; i < 25; i++)
    //     for (int j = 0; j < 3 && links[i][j]; j++) {
    //         a2b(i, links[i][j] - 1);
    //     }
    // https://blog.csdn.net/dashuniuniu/article/details/52224882
    // for (int i = 0; i < 9; i++)
    //     new BasicBlock(this);
    // entry = block_list[0];
    // int links[9][2] = {{1}, {2, 5}, {3}, {1, 4}, {}, {6, 8}, {7}, {3}, {7}};
    // for (int i = 0; i < 9; i++)
    //     for (int j = 0; j < 2 && links[i][j]; j++) {
    //         a2b(i, links[i][j]);
    //     }
    // for (int i = 0; i < 13; i++)
    //     new BasicBlock(this);
    // entry = block_list[0];
    // int links[13][3] = {{2, 5, 9}, {3},     {3, 4},   {13}, {6, 7}, {4, 8},
    //                     {8, 12},   {5, 13}, {10, 11}, {12}, {12},   {13}};
    // for (int i = 0; i < 13; i++)
    //     for (int j = 0; j < 3 && links[i][j]; j++) {
    //         a2b(i, links[i][j] - 1);
    //     }
    // https://blog.csdn.net/dashuniuniu/article/details/103275708
    // for (int i = 0; i < 12; i++)
    //     new BasicBlock(this);
    // entry = block_list[0];
    // int links[12][2] = {{1, 2}, {5},    {3, 4},  {6},  {6},    {1, 7},
    //                     {2, 7}, {8, 9}, {8, 10}, {10}, {7, 11}};
    // for (int i = 0; i < 12; i++)
    //     for (int j = 0; j < 2 && links[i][j]; j++) {
    //         a2b(i, links[i][j]);
    //     }
    for (int i = 0; i < 8; i++)
        new BasicBlock(this);
    entry = block_list[0];
    int links[8][2] = {{1}, {2}, {3, 6}, {}, {2}, {4, 7}, {5}, {5}};
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 2 && links[i][j]; j++) {
            a2b(i, links[i][j]);
        }
    computeDFSTree();
    computeSdom();
    computeIdom();
    computeDomFrontier();
}

void Function::computeReverseDFSTree(BasicBlock* exit) {
    TreeNode::Num = 0;
    int len = block_list.size();
    preOrder2DFS.resize(len);
    bool* visited = new bool[len]{};
    DFSTreeRoot = new TreeNode(exit);
    preOrder2DFS[DFSTreeRoot->num] = DFSTreeRoot;
    reverseSearch(DFSTreeRoot, visited);
    delete[] visited;
}

void Function::reverseSearch(TreeNode* node, bool* visited) {
    int n = getIndex(node->block);
    visited[n] = true;
    auto block = block_list[n];
    for (auto it = block->pred_begin(); it != block->pred_end(); it++) {
        int idx = getIndex(*it);
        if (!visited[idx]) {
            TreeNode* child = new TreeNode(*it);
            preOrder2DFS[child->num] = child;
            child->parent = node;
            node->addChild(child);
            reverseSearch(child, visited);
        }
    }
}

void Function::computeReverseSdom(BasicBlock* exit) {
    int len = block_list.size();
    sdoms.resize(len);
    int* ancestors = new int[len];
    for (int i = 0; i < len; i++) {
        sdoms[i] = i;
        ancestors[i] = -1;
    }
    for (auto it = preOrder2DFS.rbegin(); (*it)->block != exit; it++) {
        auto block = (*it)->block;
        int s = block->order;
        for (auto it1 = block->succ_begin(); it1 != block->succ_end(); it1++) {
            int z = eval((*it1)->order, ancestors);
            if (sdoms[z] < sdoms[s])
                sdoms[s] = sdoms[z];
        }
        ancestors[s] = (*it)->parent->num;
    }
    delete[] ancestors;
}

void Function::computeReverseIdom(BasicBlock* exit) {
    int len = block_list.size();
    idoms.resize(len);
    domTreeRoot = new TreeNode(exit, 0);
    preOrder2dom.resize(len);
    preOrder2dom[exit->order] = domTreeRoot;
    idoms[exit->order] = 0;
    for (auto it = preOrder2DFS.begin() + 1; it != preOrder2DFS.end(); it++) {
        int p = LCA((*it)->parent->num, sdoms[(*it)->num]);
        idoms[(*it)->num] = p;
        auto parent = preOrder2dom[p];
        TreeNode* node = new TreeNode((*it)->block, 0);
        node->parent = parent;
        parent->addChild(node);
        preOrder2dom[(*it)->num] = node;
    }
}

void Function::computeReverseDomFrontier() {
    BasicBlock* exit = new BasicBlock(this);
    for (auto b : block_list) {
        if (b->rbegin()->isRet()) {
            b->addSucc(exit);
            exit->addPred(b);
        }
    }
    computeReverseDFSTree(exit);
    computeReverseSdom(exit);
    computeReverseIdom(exit);
    for (auto block : block_list) {
        if (block->getNumOfSucc() >= 2) {
            for (auto it = block->succ_begin(); it != block->succ_end(); it++) {
                int runner = (*it)->order;
                while (runner != idoms[block->order]) {
                    preOrder2DFS[runner]->block->domFrontier.insert(block);
                    runner = idoms[runner];
                }
            }
        }
    }
    delete exit;
}

int Function::getEssential() {
    if (essential != -1)
        return essential;
    FunctionType* type = (FunctionType*)(sym_ptr->getType());
    auto paramsType = type->getParamsType();
    for (auto type : paramsType)
        if (type->isArray()) {
            essential = 1;
            return essential;
        }
    for (auto block : block_list) {
        for (auto it = block->begin(); it != block->end(); it = it->getNext()) {
            if (it->isCall()) {
                IdentifierSymbolEntry* funcSE =
                    (IdentifierSymbolEntry*)(((CallInstruction*)it)
                                                 ->getFuncSE());
                if (funcSE->isSysy() ||
                    funcSE->getName() == "llvm.memset.p0.i32") {
                    essential = 1;
                    return essential;
                } else {
                    auto func = funcSE->getFunction();
                    if (func == this)
                        continue;
                    if (func->getEssential() == 1) {
                        essential = 1;
                        return essential;
                    }
                }
            } else {
                auto def = it->getDef();
                if (def && def->getEntry()->isVariable()) {
                    auto se = (IdentifierSymbolEntry*)(def->getEntry());
                    if (se->isGlobal()) {
                        essential = 1;
                        return essential;
                    }
                }
                auto uses = it->getUse();
                for (auto use : uses) {
                    if (use && use->getEntry()->isVariable()) {
                        auto se = (IdentifierSymbolEntry*)(use->getEntry());
                        if (se->isGlobal()) {
                            essential = 1;
                            return essential;
                        }
                    }
                }
            }
        }
    }
    essential = 0;
    return essential;
}

BasicBlock* Function::getMarkBranch(BasicBlock* block) {
    set<BasicBlock*> blocks;
    while (true) {
        auto order = idoms[block->order];
        block = preOrder2dom[order]->block;
        if (blocks.count(block))
            return nullptr;
        blocks.insert(block);
        if (block->isMark())
            return block;
    }
}

void Function::genSSAGraph() {
    vector<Instruction*> temp;
    for (auto block : block_list) {
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            bool flag = in->genNode();
            if (!flag) {
                temp.push_back(in);
                nodes.push_back(in->getNode());
            } else {
                auto node = in->getNode();
                if (node &&
                    find(nodes.begin(), nodes.end(), node) == nodes.end())
                    nodes.push_back(node);
            }
        }
    }
    std::vector<Instruction*> falseIns;
    
    while(temp.size()!=0){
        for (auto in : temp) {
            bool flag;
            flag = in->reGenNode();
            if(!flag){
                falseIns.push_back(in);
            }
        }
        temp.clear();
        temp.assign(falseIns.begin(),falseIns.end());
        falseIns.clear();
    }
    // for (auto in : temp) {
    //     PhiInstruction* phi = (PhiInstruction*)in;
    //     bool flag = phi->reGenNode();
    //     assert(flag);
    // }
}

void Function::computeStores() {
    for (auto node : preOrder2dom) {
        auto block = node->block;
        for (auto it = block->pred_begin(); it != block->pred_end(); it++)
            for (auto ope : (*it)->getStores1()) {
                block->addStore(ope);
                stores.insert(ope);
            }
        for (auto ope : block->getStores())
            block->addStore1(ope);
        for (auto in = block->begin(); in != block->end(); in = in->getNext())
            if (in->isStore()) {
                block->addStore1(in->getUse()[0]);
                stores.insert(in->getUse()[0]);
            }
    }
    for (auto block : block_list) {
        for (auto it = block->pred_begin(); it != block->pred_end(); it++)
            for (auto ope : (*it)->getStores1()) {
                block->addStore1(ope);
                stores.insert(ope);
            }
    }
}

void Function::addPred(Instruction* in) {
    assert(in->isCall());
    auto func = in->getParent()->getParent();
    if (func == this)
        recur = true;
    if (preds.count(func))
        preds[func].push_back(in);
    else
        preds[func] = {in};
}

void Function::removePred(Instruction* in) {
    assert(in->isCall());
    auto func = in->getParent()->getParent();
    auto it = find(preds[func].begin(), preds[func].end(), in);
    assert(it != preds[func].end());
    preds[func].erase(it);
}
