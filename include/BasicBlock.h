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

   public:
    int order;
    std::set<BasicBlock*> domFrontier;
    BasicBlock(Function*);
    ~BasicBlock();
    void insertFront(Instruction*);
    void insertBack(Instruction*);
    void insertBefore(Instruction*, Instruction*);
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
};

#endif