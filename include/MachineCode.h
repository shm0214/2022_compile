#ifndef __MACHINECODE_H__
#define __MACHINECODE_H__
#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include "SymbolTable.h"

/* Hint:
 * MachineUnit: Compiler unit
 * MachineFunction: Function in assembly code
 * MachineInstruction: Single assembly instruction
 * MachineOperand: Operand in assembly instruction, such as immediate number,
 * register, address label */

/* Todo:
 * We only give the example code of "class BinaryMInstruction" and "class
 * AccessMInstruction" (because we believe in you !!!), You need to complete
 * other the member function, especially "output()" , After that, you can use
 * "output()" to print assembly code . */

class MachineUnit;
class MachineFunction;
class MachineBlock;
class MachineInstruction;
using LL = long long;
class MachineOperand {
   private:
    MachineInstruction* def = nullptr;
    MachineInstruction* parent;
    int type;
    LL val;            // value of immediate number
    int reg_no;         // register no
    std::string label;  // address label
    bool param = false;
    bool fpu = false;  // floating point
    float fval;
    // 用于计算栈内偏移
    int paramNo;
    int allParamNo;

   public:
    enum { IMM, VREG, REG, LABEL };
    MachineOperand(int tp, LL val, bool fpu = false);
    MachineOperand() = default;
    MachineOperand(int tp, int val, bool fpu = false);
    MachineOperand(std::string label);
    MachineOperand(int tp, float fval);
    bool operator==(const MachineOperand&) const;
    bool operator<(const MachineOperand&) const;
    bool isImm() { return this->type == IMM; };
    bool isReg() { return this->type == REG; };
    bool isVReg() { return this->type == VREG; };
    bool isLabel() { return this->type == LABEL; };
    LL getVal() { return this->val; };
    void setVal(LL val) { this->val = val; };
    float getFVal() { return this->fval; }
    void setFVal(float fval) {
        this->fval = fval;
        this->fpu = true;
    }
    void setDef(MachineInstruction* inst) { def = inst; };
    MachineInstruction* getDef() { return def; };
    uint32_t getBinVal();
    bool isFloat() { return this->fpu; }
    int getReg() { return this->reg_no; };
    void setReg(int regno) {
        this->type = REG;
        this->reg_no = regno;
    };
    std::string getLabel() { return this->label; };
    void setParent(MachineInstruction* p) { this->parent = p; };
    MachineInstruction* getParent() { return this->parent; };
    void PrintReg();
    void output();
    bool needColor() {
        return type == VREG || (type == REG && (reg_no < 11 || reg_no >= 16));
    }
    void setParam() { param = true; }
    bool isParam() { return param; }
    void setParamNo(int no) { paramNo = no; }
    void setAllParamNo(int no) { allParamNo = no; }
    int getAllParamNo() { return allParamNo; }
    int getOffset() { return 4 * allParamNo; };
    // used for local value number
    std::string toStr();
};

class MachineInstruction {
   protected:
    MachineBlock* parent;
    int no;
    int type;                             // Instruction type
    int cond = MachineInstruction::NONE;  // Instruction execution condition,
                                          // optional !!
    int op;                               // Instruction opcode
    // Instruction operand list, sorted by appearance order in assembly
    // instruction
    std::vector<MachineOperand*> def_list;
    std::vector<MachineOperand*> use_list;
    void addDef(MachineOperand* ope) { def_list.push_back(ope); };
    void addUse(MachineOperand* ope) { use_list.push_back(ope); };
    // Print execution code after printing opcode
    void PrintCond();

   public:
    enum instType {
        BINARY,
        LOAD,
        STORE,
        MOV,
        BRANCH,
        CMP,
        STACK,
        VCVT,
        VMRS,
        FUSE,
        VNEG,
        SMULL
    };
    enum condType { EQ, NE, LT, LE, GT, GE, NONE };
    virtual void output() = 0;
    virtual int latency() { return 1; }
    void setNo(int no) { this->no = no; };
    int getNo() { return no; };
    std::vector<MachineOperand*>& getDef() { return def_list; };
    std::vector<MachineOperand*>& getUse() { return use_list; };
    void insertBefore(MachineInstruction*);
    void insertAfter(MachineInstruction*);
    MachineBlock* getParent() const { return parent; };
    // 简单起见这样写了
    bool isBX() const { return type == BRANCH && op == 2; };
    bool isBL() const { return type == BRANCH && op == 1; };
    bool isB() const { return type == BRANCH && op == 0; };
    bool isBranch() const { return type == BRANCH; };
    bool isCmp() const { return type == CMP; };
    bool isLoad() const { return type == LOAD; };
    bool isStore() const { return type == STORE; };
    bool isBinary() const { return type == BINARY; };
    bool isSmull() const { return type == SMULL; };
    bool isAdd() const { return type == BINARY && op == 0; };
    bool isVAdd() const { return type == BINARY && op == 7; };
    bool isSub() const { return type == BINARY && op == 1; };
    bool isVSub() const { return type == BINARY && op == 7; }
    bool isMul() const { return type == BINARY && op == 2; };
    bool isVMul() const { return type == BINARY && op == 8; }
    bool isDiv() const { return type == BINARY && op == 3; };
    bool isDivConst() const { return type == BINARY && op == 3 && use_list[1]->getDef() && use_list[1]->getDef()->getUse()[0]->isImm(); };
    bool isMod() const { return type == BINARY && op == 4; };
    
    bool isMov() const { return type == MOV && op == 0; };
    bool isVMov() const { return type == MOV && op == 3; };
    bool isVMovf32() const { return type == MOV && op == 4; };
    bool isCondMov() const { return type == MOV && op == 0 && cond != NONE; };
    bool isPush() const { return type == STACK && op == 0; };
    bool isStack() const { return type == STACK; }
    void replaceUse(MachineOperand* old, MachineOperand* new_);
    void replaceDef(MachineOperand* old, MachineOperand* new_);
    int getCond() const { return cond; }
    void setCond(int cond) { this->cond = cond; }
    void setParent(MachineBlock* block) { this->parent = block; }
    bool isAddZero() const {
        return isAdd() && use_list[1]->isImm() && use_list[1]->getVal() == 0;
    }
    int getType() { return type; }
    int getOp() { return op; }
    bool isSpecial() const {
        for (auto& ope : def_list) {
            if (ope->isReg() && ope->getReg() >= 11 && ope->getReg() <= 15) {
                return true;
            }
        }
        for (auto& ope : use_list) {
            if (ope->isReg() && ope->getReg() >= 11 && ope->getReg() <= 15) {
                return true;
            }
        }
        return false;
    }
    virtual std::string getHash() { return ""; }
};

class VNegMInstruction : public MachineInstruction {
   public:
    enum opType { S32, F32 };
    VNegMInstruction(MachineBlock* p,
                     int op,
                     MachineOperand* dst,
                     MachineOperand* src);
    void output();
    int latency();
};

class FuseMInstruction : public MachineInstruction {
   public:
    enum opType { MLA, MLS, VMLA, VMLS };
    FuseMInstruction(MachineBlock* p,
                     int op,
                     MachineOperand* dst,
                     MachineOperand* src1,
                     MachineOperand* src2,
                     MachineOperand* src3);
    void output();
    int latency();
};

class BinaryMInstruction : public MachineInstruction {
   public:
    enum opType { ADD, SUB, MUL, DIV, MOD, AND, OR, VADD, VSUB, VMUL, VDIV };
    BinaryMInstruction(MachineBlock* p,
                       int op,
                       MachineOperand* dst,
                       MachineOperand* src1,
                       MachineOperand* src2,
                       int cond = MachineInstruction::NONE);
    void output();
    int latency();
    std::string getHash();
};

class SmullMInstruction : public MachineInstruction {
   public:
    SmullMInstruction(MachineBlock* p,
                       MachineOperand* dst,
                       MachineOperand* dst1,
                       MachineOperand* src1,
                       MachineOperand* src2,
                       int cond = MachineInstruction::NONE);
    void output();
    int latency();
};

class LoadMInstruction : public MachineInstruction {
   private:
    bool needModify;

   public:
    enum opType { LDR, VLDR };
    LoadMInstruction(MachineBlock* p,
                     int op,
                     MachineOperand* dst,
                     MachineOperand* src1,
                     MachineOperand* src2 = nullptr,
                     int cond = MachineInstruction::NONE);
    void output();
    void setNeedModify() { needModify = true; }
    bool isNeedModify() { return needModify; }
    int latency();
    std::string getHash();
};

class StoreMInstruction : public MachineInstruction {
   public:
    enum opType { STR, VSTR };
    StoreMInstruction(MachineBlock* p,
                      int op,
                      MachineOperand* src1,
                      MachineOperand* src2,
                      MachineOperand* src3 = nullptr,
                      int cond = MachineInstruction::NONE);
    void output();
    int latency();
};

class MovMInstruction : public MachineInstruction {
   public:
    enum opType { MOV, MVN, MOVT, VMOV, VMOVF32, MOVLSL, MOVLSR, MOVASR };
    MovMInstruction(MachineBlock* p,
                    int op,
                    MachineOperand* dst,
                    MachineOperand* src,
                    int cond = MachineInstruction::NONE,
                    MachineOperand* num = nullptr);
    void output();
    int latency();
    std::string getHash();
};

class BranchMInstruction : public MachineInstruction {
   public:
    enum opType { B, BL, BX };
    BranchMInstruction(MachineBlock* p,
                       int op,
                       MachineOperand* dst,
                       int cond = MachineInstruction::NONE);
    void output();
    int latency();
};

class CmpMInstruction : public MachineInstruction {
   public:
    enum opType { CMP, VCMP };
    CmpMInstruction(MachineBlock* p,
                    int op,
                    MachineOperand* src1,
                    MachineOperand* src2,
                    int cond = MachineInstruction::NONE);
    void output();
    int latency();
};

class StackMInstruction : public MachineInstruction {
   public:
    enum opType { PUSH, POP, VPUSH, VPOP };
    StackMInstruction(MachineBlock* p,
                      int op,
                      std::vector<MachineOperand*> srcs,
                      MachineOperand* src = nullptr,
                      MachineOperand* src1 = nullptr,
                      int cond = MachineInstruction::NONE);
    void output();
    int latency();
};

class VcvtMInstruction : public MachineInstruction {
   public:
    enum opType { S2F, F2S };
    VcvtMInstruction(MachineBlock* p,
                     int op,
                     MachineOperand* dst,
                     MachineOperand* src,
                     int cond = MachineInstruction::NONE);
    void output();
    int latency();
};

class VmrsMInstruction : public MachineInstruction {
   public:
    VmrsMInstruction(MachineBlock* p);
    void output();
    int latency();
};

class MachineBlock {
   private:
    MachineFunction* parent;
    int no;
    std::vector<MachineBlock*> pred, succ;
    std::vector<MachineInstruction*> inst_list;
    std::set<MachineOperand*> live_in;
    std::set<MachineOperand*> live_out;
    std::set<MachineOperand*> def_in;
    std::set<MachineOperand*> def_out;
    int cmpCond;
    static int label;

   public:
    std::vector<MachineInstruction*>& getInsts() { return inst_list; };
    std::vector<MachineInstruction*>::iterator begin() {
        return inst_list.begin();
    };
    std::vector<MachineInstruction*>::reverse_iterator rbegin() {
        return inst_list.rbegin();
    };
    std::vector<MachineInstruction*>::iterator end() {
        return inst_list.end();
    };
    std::vector<MachineInstruction*>::iterator nonbranch_end();
    MachineBlock(MachineFunction* p, int no) {
        this->parent = p;
        this->no = no;
    };
    void InsertInst(MachineInstruction* inst) {
        this->inst_list.push_back(inst);
        inst->setParent(this);
    };
    void addPred(MachineBlock* p) { this->pred.push_back(p); };
    void addSucc(MachineBlock* s) { this->succ.push_back(s); };
    std::set<MachineOperand*>& getLiveIn() { return live_in; };
    std::set<MachineOperand*>& getLiveOut() { return live_out; };
    std::set<MachineOperand*>& getDefIn() { return def_in; };
    std::set<MachineOperand*>& getDefOut() { return def_out; };
    std::vector<MachineBlock*>& getPreds() { return pred; };
    std::vector<MachineBlock*>& getSuccs() { return succ; };
    void output();
    int getCmpCond() const { return cmpCond; };
    void setCmpCond(int cond) { cmpCond = cond; };
    int getSize() const { return inst_list.size(); };
    MachineFunction* getParent() const { return parent; };
    bool isBefore(MachineInstruction* a, MachineInstruction* b);
    void replace(MachineInstruction* before, MachineInstruction* after);
    void remove(MachineInstruction* ins);
    MachineInstruction* getNext(MachineInstruction* in);
    std::string getLabel();
    void cleanSucc();
    void removePred(MachineBlock* block);
    void removeSucc(MachineBlock* block);
    int getNo() const { return no; }
    // insert a before b
    void insertBefore(MachineInstruction* a, MachineInstruction* b);
    void insertFront(MachineInstruction* in);
};

class MachineFunction {
   private:
    MachineUnit* parent;
    std::vector<MachineBlock*> block_list;
    int stack_size;
    std::set<int> saved_regs;
    std::set<int> saved_fpregs;
    SymbolEntry* sym_ptr;
    int paramsNum;
    MachineBlock* entry;
    std::map<int, MachineBlock*> no2Block;

    bool need_align;

   public:
    std::vector<MachineBlock*>& getBlocks() { return block_list; };
    std::vector<MachineBlock*>::iterator begin() { return block_list.begin(); };
    std::vector<MachineBlock*>::iterator end() { return block_list.end(); };
    MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr);
    /* HINT:
     * Alloc stack space for local variable;
     * return current frame offset ;
     * we store offset in symbol entry of this variable in function
     * AllocInstruction::genMachineCode() you can use this function in
     * LinearScan::genSpillCode() */
    int AllocSpace(int size) {
        this->stack_size += size;
        return this->stack_size;
    };
    void InsertBlock(MachineBlock* block) {
        this->block_list.push_back(block);
        no2Block[block->getNo()] = block;
    };
    void addSavedRegs(int regno);
    void output();
    std::vector<MachineOperand*> getSavedRegs();
    std::vector<MachineOperand*> getSavedFpRegs();
    int getParamsNum() const { return paramsNum; };
    MachineUnit* getParent() const { return parent; };
    void setEntry(MachineBlock* entry) { this->entry = entry; }
    MachineBlock* getEntry() { return entry; };
    SymbolEntry* getSymbolEntry() { return sym_ptr; };
    int getSize() const {
        int res = 0;
        for (auto block : block_list)
            res += block->getSize();
        return res;
    };
    void removeBlock(MachineBlock* block);
    MachineBlock* getBlock(int no) { return no2Block[no]; }
    MachineBlock* getNext(MachineBlock* block);
    // insert b after a
    void InsertAfter(MachineBlock* a, MachineBlock* b);
    bool needAlign() { return need_align; }
};

class MachineUnit {
   private:
    std::vector<SymbolEntry*> global_list;
    std::vector<MachineFunction*> func_list;
    void PrintGlobalDecl();
    int n;

   public:
    std::vector<MachineFunction*>& getFuncs() { return func_list; };
    std::vector<MachineFunction*>::iterator begin() {
        return func_list.begin();
    };
    std::vector<MachineFunction*>::iterator end() { return func_list.end(); };
    void InsertFunc(MachineFunction* func) { func_list.push_back(func); };
    void output();
    void insertGlobal(SymbolEntry*);
    void printGlobal();
    int getN() const { return n; };
};

#endif