#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <map>
#include <sstream>
#include <vector>
#include "AsmBuilder.h"
#include "Operand.h"
#include "SSAGraph.h"
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
    bool isCall() const { return instType == CALL; }
    bool isStore() const { return instType == STORE; }
    bool isPhi() const { return instType == PHI; };
    bool isBin() const { return instType == BINARY; };
    bool isLoad() const { return instType == LOAD; };
    bool isCmp() const { return instType == CMP; };
    bool isGep() const { return instType == GEP; };
    void setParent(BasicBlock*);
    void setNext(Instruction*);
    void setPrev(Instruction*);
    void remove();
    Instruction* getNext();
    Instruction* getPrev();
    virtual void output() const = 0;
    MachineOperand* genMachineOperand(Operand*);
    MachineOperand* genMachineFloatOperand(Operand*);
    MachineOperand* genMachineReg(int reg);
    MachineOperand* genMachineFReg(int freg);
    MachineOperand* genMachineVReg(bool fpu = false);
    MachineOperand* genMachineImm(int val);
    MachineOperand* genMachineLabel(int block_no);
    virtual void genMachineCode(AsmBuilder*) = 0;
    int getInstType() { return instType; }
    bool isEssential();
    void setMark() { mark = true; }
    void unsetMark() { mark = false; }
    bool isMark() const { return mark; }
    virtual std::vector<Operand*> getUse() { return std::vector<Operand*>(); }
    virtual Operand* getDef() { return nullptr; }
    virtual void replaceUse(Operand* old, Operand* new_) {}
    virtual void replaceDef(Operand* new_) {}
    std::vector<Operand*> getOperands() { return operands; }
    virtual bool genNode() { return true; }
    SSAGraphNode* getNode() { return node; }
    virtual std::string getHash() { return ""; }
    bool isIntMul();
    bool isIntDiv();
    virtual bool isConstExp() { return false; }
    double getConstVal() { return constVal; }
    bool isComAndAsso() {
        if (isBin() && opcode >= 1 && opcode <= 4)
            return true;
        return false;
    }
    int getOpcode() const { return opcode; }
    bool hasEqualOp(Instruction* in) const {
        if ((int)instType == in->getInstType())
            if ((int)opcode == in->getOpcode())
                return true;
        return false;
    }
    bool isAdd() { return isBin() && opcode == 1; }
    // shallow copy
    virtual Instruction* copy() = 0;
    virtual void setDef(Operand* def) {}
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
        PHI,
        FPTOSI,  // floating point to signed int
        SITOFP,  // signed int to floating point
        BITCAST,
        SHL,
        ASHR,
    };

   protected:
    unsigned instType;
    unsigned opcode;
    Instruction* prev;
    Instruction* next;
    BasicBlock* parent;
    std::vector<Operand*> operands;
    bool mark;
    SSAGraphNode* node;
    double constVal;
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
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }

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
    std::string getHash();
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
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
    int getOp() { return opcode; }
    Operand* getDef() { return operands[0]; }
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1], operands[2]});
    }
    bool genNode();
    std::string getHash();
    bool isConstExp();
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
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
    int getOp() { return opcode; }
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    Operand* getDef() { return operands[0]; }
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1], operands[2]});
    }
    bool genNode();
    std::string getHash();
    bool isConstExp();
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
};

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
    SymbolEntry* getFuncSE() { return func; }
    bool genNode();
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        dst = def;
        def->setDef(this);
    }
    // used for auto inline
    void addPred();
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
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
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
    std::string getHash();
    bool isConstExp();
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
};

class GepInstruction : public Instruction {
   private:
    bool paramFirst;
    bool first;
    bool last;
    Operand* init;
    int off;
    // 由于数组初始化使用+4 部分未生成汇编 值编号会重用因此标记
    bool noAsm;

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
    void setInit(Operand* init, int off = 0) {
        this->init = init;
        this->off = off;
    };
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1], operands[2]});
    }
    Operand* getDef() { return operands[0]; }
    void replaceDef(Operand* new_);
    void replaceUse(Operand* old, Operand* new_);
    bool genNode();
    std::string getHash();
    bool hasNoAsm() { return noAsm; }
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
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
    Operand* getSrc(BasicBlock* block);
    Operand* getDef() { return dst; }
    void genMachineCode(AsmBuilder*) {}
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    Operand* getOriginDef() { return originDef; }
    void replaceOriginDef(Operand* new_);
    void changeSrcBlock(
        std::map<BasicBlock*, std::vector<BasicBlock*>> changes);
    std::vector<Operand*> getUse() {
        std::vector<Operand*> ret;
        for (auto ope : operands)
            if (ope != operands[0])
                ret.push_back(ope);
        return ret;
    }
    bool genNode();
    bool reGenNode();
    std::string getHash();
    std::map<BasicBlock*, Operand*>& getSrcs() { return srcs; }
    void cleanUse();
    Instruction* copy();
    void setDef(Operand* def) {
        dst = def;
        operands[0] = def;
        def->setDef(this);
    }
};

class FptosiInstruction : public Instruction {
   private:
    Operand* dst;
    Operand* src;

   public:
    FptosiInstruction(Operand* dst,
                      Operand* src,
                      BasicBlock* insert_bb = nullptr);
    ~FptosiInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
};

class SitofpInstruction : public Instruction {
   private:
    Operand* dst;
    Operand* src;

   public:
    SitofpInstruction(Operand* dst,
                      Operand* src,
                      BasicBlock* insert_bb = nullptr);
    ~SitofpInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
};

class BitcastInstruction : public Instruction {
   private:
    Operand* dst;
    Operand* src;

   public:
    BitcastInstruction(Operand* dst,
                       Operand* src,
                       BasicBlock* insert_bb = nullptr);
    ~BitcastInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1]});
    }
    bool genNode();
    Instruction* copy();
    Operand* getDef() { return operands[0]; }
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
    void replaceUse(Operand* old, Operand* new_);
};

class ShlInstruction : public Instruction {
   public:
    ShlInstruction(Operand* dst,
                   Operand* src,
                   Operand* num,
                   BasicBlock* insert_bb = nullptr);
    ~ShlInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1], operands[2]});
    }
    Operand* getDef() { return operands[0]; }
    bool genNode();
    std::string getHash();
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    bool isConstExp();
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
};

class AshrInstruction : public Instruction {
   public:
    AshrInstruction(Operand* dst,
                    Operand* src,
                    Operand* num,
                    BasicBlock* insert_bb = nullptr);
    ~AshrInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    std::vector<Operand*> getUse() {
        return std::vector<Operand*>({operands[1], operands[2]});
    }
    Operand* getDef() { return operands[0]; }
    bool genNode();
    std::string getHash();
    void replaceUse(Operand* old, Operand* new_);
    void replaceDef(Operand* new_);
    bool isConstExp();
    Instruction* copy();
    void setDef(Operand* def) {
        operands[0] = def;
        def->setDef(this);
    }
};

#endif