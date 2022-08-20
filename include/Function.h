#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "AsmBuilder.h"
#include "Ast.h"
#include "BasicBlock.h"
#include "SSAGraph.h"
#include "SymbolTable.h"
class Unit;

struct TreeNode {
    // num only use for dfs tree node
    static int Num;
    int num;
    BasicBlock* block;
    std::vector<TreeNode*> children;
    TreeNode* parent = nullptr;
    // only use for dfs tree node
    TreeNode(BasicBlock* block) : block(block) {
        num = Num++;
        block->order = num;
    }
    // only use for dom tree node
    TreeNode(BasicBlock* block, int num) : block(block) {
        this->num = block->order;
    }
    void addChild(TreeNode* child) { children.push_back(child); }
    // only use for dom tree node
    int getHeight() {
        int height = 0;
        TreeNode* temp = this;
        while (temp) {
            height++;
            temp = temp->parent;
        }
        return height;
    }
};

class Function {
    typedef std::vector<BasicBlock*>::iterator iterator;
    typedef std::vector<BasicBlock*>::reverse_iterator reverse_iterator;

   private:
    std::vector<BasicBlock*> block_list;
    SymbolEntry* sym_ptr;
    BasicBlock* entry;
    Unit* parent;
    TreeNode* DFSTreeRoot;
    TreeNode* domTreeRoot;
    // preOrder2DFS order-> dfs tree node
    std::vector<TreeNode*> preOrder2DFS;
    // preOrder2dom order-> dom tree node
    std::vector<TreeNode*> preOrder2dom;
    // sdoms idoms order->order
    std::vector<int> sdoms;
    std::vector<int> idoms;
    // -1 for not calculate
    // 1 for essential
    // 0 not essential
    // a function is essential if it is a sysy/memset function or it has a array param or it call a essential function
    int essential = -1;
    std::vector<SSAGraphNode*> nodes;
    std::set<Operand*> stores;
    // 用于mem2reg 有调用其他函数的话则为true
    bool call;
    std::map<Function*, std::vector<Instruction*>> preds;
    bool recur;
    // used for auto inline
    int instNum;
    std::set<Operand*> storedGlobals;

   public:
    Function() {}
    Function(Unit*, SymbolEntry*);
    ~Function();
    void insertBlock(BasicBlock* bb) { block_list.push_back(bb); };
    BasicBlock* getEntry() { return entry; };
    void setEntry(BasicBlock* entry) { this->entry = entry; };
    void remove(BasicBlock* bb);
    void output();
    std::vector<BasicBlock*>& getBlockList() { return block_list; };
    iterator begin() { return block_list.begin(); };
    iterator end() { return block_list.end(); };
    reverse_iterator rbegin() { return block_list.rbegin(); };
    reverse_iterator rend() { return block_list.rend(); };
    SymbolEntry* getSymPtr() { return sym_ptr; };
    void genMachineCode(AsmBuilder*);
    std::vector<std::vector<int>> getBlockMap();
    void computeDFSTree();
    void search(TreeNode* node, bool* visited);
    int getIndex(BasicBlock* block) {
        return std::find(block_list.begin(), block_list.end(), block) -
               block_list.begin();
    }
    int eval(int i, int* ancestors);
    void computeSdom();
    int LCA(int i, int j);
    void computeIdom();
    void domTest();
    void computeDomFrontier();
    TreeNode* getDomNode(BasicBlock* b) { return preOrder2dom[b->order]; }
    void dfs(AsmBuilder* builder,
             BasicBlock* block,
             std::set<BasicBlock*>& v,
             std::map<BasicBlock*, MachineBlock*>& map);
    void dfs1(BasicBlock* block, std::set<BasicBlock*>& v);
    void computeReverseDFSTree(BasicBlock* exit);
    void reverseSearch(TreeNode* node, bool* visited);
    void computeReverseSdom(BasicBlock* exit);
    void computeReverseIdom(BasicBlock* exit);
    void computeReverseDomFrontier();
    int getEssential();
    BasicBlock* getMarkBranch(BasicBlock* block);
    void computeStores();
    std::set<Operand*>& getStores() { return stores; }
    bool hasCall() { return call; }
    void setHasCall() { call = true; }
    std::map<Function*, std::vector<Instruction*>>& getPreds() {
        return preds;
    };
    void addPred(Instruction* in);
    void removePred(Instruction* in);
    bool hasRecur() { return recur; }
    void setInstNum(int num) { instNum = num; }
    int getInstNum() { return instNum; }
    void addStoredGlobal(Operand* ope) { storedGlobals.insert(ope); }
    std::set<Operand*> getStoredGlobals() { return storedGlobals; }
    void genSSAGraph();
    std::vector<SSAGraphNode*>& getSSAGraph(){return nodes;};
    void addNodeToSSAGraph(SSAGraphNode* node){nodes.push_back(node);};
};

#endif
