#include "Function.h"
#include <list>
#include <vector>
#include "Type.h"
#include "Unit.h"

using namespace std;

extern FILE* yyout;

Function::Function(Unit* u, SymbolEntry* s) {
    u->insertFunc(this);
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
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

void Function::output() const {
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
    std::list<BasicBlock*> q;
    q.push_back(entry);
    v.insert(entry);

    while (!q.empty()) {
        auto bb = q.front();
        q.pop_front();
        bb->output();
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++) {
            if (v.find(*succ) == v.end()) {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }
    fprintf(yyout, "}\n");
}

void Function::genMachineCode(AsmBuilder* builder) {
    auto cur_unit = builder->getUnit();
    auto cur_func = new MachineFunction(cur_unit, this->sym_ptr);
    builder->setFunction(cur_func);
    std::map<BasicBlock*, MachineBlock*> map;
    for (auto block : block_list) {
        block->genMachineCode(builder);
        map[block] = builder->getBlock();
    }
    // Add pred and succ for every block
    for (auto block : block_list) {
        auto mblock = map[block];
        for (auto pred = block->pred_begin(); pred != block->pred_end(); pred++)
            mblock->addPred(map[*pred]);
        for (auto succ = block->succ_begin(); succ != block->succ_end(); succ++)
            mblock->addSucc(map[*succ]);
    }
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
    //     cout << preOrder2DFS[block->order]->block->getNo()<< ": ";
    //     for (auto c : block->domFrontier) {
    //         cout << preOrder2DFS[c->order]->block->getNo()<< " ";
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
    for (int i = 0; i < 12; i++)
        new BasicBlock(this);
    entry = block_list[0];
    int links[12][2] = {{1, 2}, {5},    {3, 4},  {6},  {6},    {1, 7},
                        {2, 7}, {8, 9}, {8, 10}, {10}, {7, 11}};
    for (int i = 0; i < 12; i++)
        for (int j = 0; j < 2 && links[i][j]; j++) {
            a2b(i, links[i][j]);
        }
    computeDFSTree();
    computeSdom();
    computeIdom();
    computeDomFrontier();
}