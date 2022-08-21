#ifndef __BASIC_BLOCK_H__
#define __BASIC_BLOCK_H__
#include <set>
#include <vector>
#include "AsmBuilder.h"
#include "Instruction.h"

class Function;

class BasicBlock {
    typedef std::vector<BasicBlock*>::iterator bb_iterator;

   private:
    std::vector<BasicBlock*> pred, succ;
    Instruction* head;
    Function* parent;
    int no;
    // for phi::changeSrcBlock
    std::map<BasicBlock*, BasicBlock*> phiBlocks;
    bool mark;
    // 不含这个块内的store
    std::set<Operand*> stores;
    // 包含这个块内的store
    std::set<Operand*> stores1;
    // used for entry
    Instruction* lastAlloc;

   public:
    int order;
    std::set<BasicBlock*> domFrontier;
    BasicBlock(Function*);
    ~BasicBlock();
    void insertFront(Instruction*);
    void insertBack(Instruction*);
    void insertBefore(Instruction*, Instruction*);
    void insertAfter(Instruction*, Instruction*);
    void remove(Instruction*);
    bool empty() const { return head->getNext() == head; }
    void output() const;
    bool succEmpty() const { return succ.empty(); };
    bool predEmpty() const { return pred.empty(); };
    void addSucc(BasicBlock*, bool first = false);
    void removeSucc(BasicBlock*);
    void removeSuccFromEnd(BasicBlock*);
    void addPred(BasicBlock*);
    void removePred(BasicBlock*);
    void removePredFromEnd(BasicBlock*);
    int getNo() { return no; };
    Function* getParent() { return parent; };
    Instruction* begin() { return head->getNext(); };
    Instruction* end() { return head; };
    Instruction* rbegin() { return head->getPrev(); };
    Instruction* rend() { return head; };
    bb_iterator succ_begin() { return succ.begin(); };
    bb_iterator succ_end() { return succ.end(); };
    bb_iterator pred_begin() { return pred.begin(); };
    bb_iterator pred_end() { return pred.end(); };
    int getNumOfPred() const { return pred.size(); };
    int getNumOfSucc() const { return succ.size(); };
    void genMachineCode(AsmBuilder*);
    void cleanSucc();
    void cleanMark();
    void insertPhiInstruction(Operand* operand);
    void cleanPhiBlocks() { phiBlocks.clear(); }
    std::map<BasicBlock*, BasicBlock*>& getPhiBlocks() { return phiBlocks; }
    std::vector<BasicBlock*> getSucc() { return succ; }
    std::vector<BasicBlock*> getPred() { return pred; }
    void deleteBack(int num = 1);
    void setMark() { mark = true; }
    void unsetMark() { mark = false; }
    bool isMark() { return mark; }
    bool isBefore(Instruction* a, Instruction* b);
    std::set<Operand*>& getStores() { return stores; }
    std::set<Operand*>& getStores1() { return stores1; }
    void addStore(Operand* ope) { stores.insert(ope); }
    void addStore1(Operand* ope) { stores1.insert(ope); }
    void removeStore(Operand* ope) { stores.erase(ope); }
    bool inStore(Operand* ope) { return stores.count(ope); }
    void replaceIns(Instruction* old, Instruction* new_);
    void addAlloc(Instruction* alloc);
};

#endif
