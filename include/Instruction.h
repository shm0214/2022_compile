#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <map>
#include <sstream>
#include <vector>
#include "AsmBuilder.h"
#include "Operand.h"
#include "Type.h"

class SSAGraphNode;
class BasicBlock;

class Instruction {
   public:
    Instruction(unsigned instType, BasicBlock* insert_bb = nullptr);
    virtual ~Instruction();
    BasicBlock* getParent();
    bool isUncond() const { return instType == UNCOND; };
    bool isCond() const { return instType == COND; };
    bool isAlloc() const { return instType == ALLOCA; };
    bool isRet() const { return instType == RET; };
    bool isStore() const { return instType == STORE; };
    bool isBin() const {return instType == BINARY; };
    bool isPhi() const { return instType == PHI; };
    bool isCmp() const {return instType == CMP;};
    void setParent(BasicBlock*);
    void setNext(Instruction*);
    void setPrev(Instruction*);
    Instruction* getNext();
    Instruction* getPrev();
    virtual void output() const = 0;
    MachineOperand* genMachineOperand(Operand*);
    MachineOperand* genMachineReg(int reg);
    MachineOperand* genMachineVReg();
    MachineOperand* genMachineImm(int val);
    MachineOperand* genMachineLabel(int block_no);
    virtual void genMachineCode(AsmBuilder*) = 0;
    int getInstType() { return instType; }
    virtual std::vector<Operand*> getUse() { return std::vector<Operand*>(); }
    virtual Operand* getDef() { return nullptr; }
    virtual void replaceUse(Operand* old, Operand* new_) {}
    virtual void replaceDef(Operand* new_) {}
    virtual bool genNode() { return true; }
    SSAGraphNode* getNode() { return node; }
    void setNode(SSAGraphNode* node){this->node=node;};
    virtual Instruction* copy() = 0;
   protected:
    unsigned instType;
    unsigned opcode;
    Instruction* prev;
    Instruction* next;
    BasicBlock* parent;
    std::vector<Operand*> operands;
    enum {
        BINARY,
        COND,
        UNCOND,
        RET,
        LOAD,
        STORE,
        CMP,
        ALLOCA,
        CALL,
        ZEXT,
        XOR,
        GEP,
        PHI
    };
    SSAGraphNode* node;
};

// meaningless instruction, used as the head node of the instruction list.
class DummyInstruction : public Instruction {
   public:
    DummyInstruction() : Instruction(-1, nullptr){};
    void output() const {};
    void genMachineCode(AsmBuilder*){};
    Instruction* copy() { return nullptr; }
};

class AllocaInstruction : public Instruction {
   public:
    AllocaInstruction(Operand* dst,
                      SymbolEntry* se,
                      BasicBlock* insert_bb = nullptr);
    ~AllocaInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Operand* getDef() { return operands[0]; }
    void replaceDef(Operand* new_);
    bool isArray() { return se->getType()->isArray(); }
    bool genNode();
    Instruction* copy();

   private:
    SymbolEntry* se;
};

class LoadInstruction : public Instruction {
   public:
    LoadInstruction(Operand* dst,
                    Operand* src_addr,
                    BasicBlock* insert_bb = nullptr);
    ~LoadInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Operand* getDef() { return operands[0]; }
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1]});
    }
    bool genNode();
    Instruction* copy();
};

class StoreInstruction : public Instruction {
   public:
    StoreInstruction(Operand* dst_addr,
                     Operand* src,
                     BasicBlock* insert_bb = nullptr);
    ~StoreInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    void replaceUse(Operand* old, Operand* new_);
    Operand* getDstAddr() { return operands[0]; }
    Operand* getSrc() { return operands[1]; }
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[0], operands[1]});
    }
    Instruction* copy();
};

class BinaryInstruction : public Instruction {
   public:
    BinaryInstruction(unsigned opcode,
                      Operand* dst,
                      Operand* src1,
                      Operand* src2,
                      BasicBlock* insert_bb = nullptr);
    ~BinaryInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    enum { SUB, ADD, AND, OR, MUL, DIV, MOD };
    Operand* getDef() { return operands[0]; }
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1], operands[2]});
    }
    bool isAdd(){return this->opcode==ADD;};
    bool isSub(){return this->opcode==SUB;};
    bool isMul(){return this->opcode==MUL;};
    bool isDIV(){return this->opcode==DIV;};
    bool genNode();
    Instruction* copy();
};

class CmpInstruction : public Instruction {
   public:
    CmpInstruction(unsigned opcode,
                   Operand* dst,
                   Operand* src1,
                   Operand* src2,
                   BasicBlock* insert_bb = nullptr);
    ~CmpInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    enum { E, NE, L, LE, G, GE };
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1], operands[2]});
    }
    Operand* getDef() { return operands[0]; }
    bool genNode();
    Instruction* copy();
};

// unconditional branch
class UncondBrInstruction : public Instruction {
   public:
    UncondBrInstruction(BasicBlock*, BasicBlock* insert_bb = nullptr);
    void output() const;
    void setBranch(BasicBlock*);
    BasicBlock* getBranch();
    void genMachineCode(AsmBuilder*);
    Instruction* copy();

   protected:
    BasicBlock* branch;
};

// conditional branch
class CondBrInstruction : public Instruction {
   private:
    BasicBlock* originTrue;
    BasicBlock* originFalse;

   public:
    CondBrInstruction(BasicBlock*,
                      BasicBlock*,
                      Operand*,
                      BasicBlock* insert_bb = nullptr);
    ~CondBrInstruction();
    void output() const;
    void setTrueBranch(BasicBlock*);
    BasicBlock* getTrueBranch();
    void setFalseBranch(BasicBlock*);
    BasicBlock* getFalseBranch();
    void genMachineCode(AsmBuilder*);
    void replaceUse(Operand* old, Operand* new_);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[0]});
    }
    Instruction* copy();
    BasicBlock* getOriginTrue() { return originTrue; }
    BasicBlock* getOriginFalse() { return originFalse; }
    void cleanOriginTrue() { originTrue = nullptr; }
    void cleanOriginFalse() { originFalse = nullptr; }


   protected:
    BasicBlock* true_branch;
    BasicBlock* false_branch;
};

class RetInstruction : public Instruction {
   public:
    RetInstruction(Operand* src, BasicBlock* insert_bb = nullptr);
    ~RetInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        if (operands.size())
            return std::vector<Operand*>({operands[0]});
        return std::vector<Operand*>();
    }
    bool genNode();
    Instruction* copy();
};

class CallInstruction : public Instruction {
   private:
    SymbolEntry* func;
    Operand* dst;

   public:
    CallInstruction(Operand* dst,
                    SymbolEntry* func,
                    std::vector<Operand*> params,
                    BasicBlock* insert_bb = nullptr);
    ~CallInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Operand* getDef() { return operands[0]; }
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        std::vector<Operand*> vec;
        for (auto it = operands.begin() + 1; it != operands.end(); it++)
            vec.push_back(*it);
        return vec;
    }
    bool genNode();
    Instruction* copy();
};

class ZextInstruction : public Instruction {
   public:
    ZextInstruction(Operand* dst,
                    Operand* src,
                    BasicBlock* insert_bb = nullptr);
    ~ZextInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Operand* getDef() { return operands[0]; }
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1]});
    }
    bool genNode();
    Instruction* copy();
};

class XorInstruction : public Instruction {
   public:
    XorInstruction(Operand* dst, Operand* src, BasicBlock* insert_bb = nullptr);
    ~XorInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Operand* getDef() { return operands[0]; }
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1]});
    }
    bool genNode();
    Instruction* copy();
};

class GepInstruction : public Instruction {
   private:
    bool paramFirst;
    bool first;
    bool last;
    Operand* init;

   public:
    GepInstruction(Operand* dst,
                   Operand* arr,
                   Operand* idx,
                   BasicBlock* insert_bb = nullptr,
                   bool paramFirst = false);
    ~GepInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    void setFirst() { first = true; };
    void setLast() { last = true; };
    Operand* getInit() const { return init; };
    void setInit(Operand* init) { this->init = init; };
    Operand* getDef() { return operands[0]; }
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1], operands[2]});
    }
    bool genNode();
    Instruction* copy();
};

class PhiInstruction : public Instruction {
   private:
    Operand* originDef;
    Operand* dst;
    std::map<BasicBlock*, Operand*> srcs;

   public:
    PhiInstruction(Operand* dst, BasicBlock* insert_bb = nullptr);
    ~PhiInstruction();
    void output() const;
    void addSrc(BasicBlock* block, Operand* src);
    void removeSrc(BasicBlock* block);
    Operand* getSrc(BasicBlock* block);
    bool findSrc(BasicBlock* block);
    Operand* getDef() { return dst; }
    void genMachineCode(AsmBuilder*) {}
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    Operand* getOriginDef() { return originDef; }
    void replaceOriginDef(Operand* new_);
    void changeSrcBlock(std::map<BasicBlock*, std::vector<BasicBlock*>> changes);
    std::vector<Operand*> getUse() {
        std::vector<Operand*> ret;
        for (auto ope : operands)
            if (ope != operands[0])
                ret.push_back(ope);
        return ret;
    }
    bool genNode();
    bool reGenNode();
    Instruction* copy();
};

#endif