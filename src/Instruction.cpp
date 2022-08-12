#include "Instruction.h"
#include <assert.h>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>
#include "BasicBlock.h"
#include "Function.h"
#include "Type.h"
extern FILE* yyout;

Instruction::Instruction(unsigned instType, BasicBlock* insert_bb) {
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr) {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
    mark = false;
    node = nullptr;
    constVal = 0;
}

Instruction::~Instruction() {
    parent->remove(this);
}

bool Instruction::isEssential() {
    // return value
    if (isRet()) {
        if (getUse().empty())
            return true;
        auto preds = parent->getParent()->getPreds();
        if (preds.empty())
            return true;
        // 只要有接收ret值的就要返回true
        for (auto it : preds)
            for (auto in : it.second)
                if (in->getDef()->usersNum())
                    return true;

        return false;
    }
    // input/output
    if (isCall()) {
        IdentifierSymbolEntry* funcSE =
            (IdentifierSymbolEntry*)(((CallInstruction*)this)->getFuncSE());
        if (funcSE->isSysy() || funcSE->getName() == "llvm.memset.p0.i32") {
            return true;
        } else {
            auto func = funcSE->getFunction();
            if (func->getEssential() == 1) {
                return true;
            }
        }
    }
    if (isStore()) {
        return true;
    }
    return false;
}

bool Instruction::isIntMul() {
    if (instType == BINARY && opcode == BinaryInstruction::MUL)
        if (operands[0]->getType()->isInt())
            return true;
    return false;
}

bool Instruction::isIntDiv() {
    if (instType == BINARY && opcode == BinaryInstruction::DIV)
        if (operands[0]->getType()->isInt())
            return true;
    return false;
}

BasicBlock* Instruction::getParent() {
    return parent;
}

void Instruction::setParent(BasicBlock* bb) {
    parent = bb;
}

void Instruction::setNext(Instruction* inst) {
    next = inst;
}

void Instruction::setPrev(Instruction* inst) {
    prev = inst;
}

Instruction* Instruction::getNext() {
    return next;
}

Instruction* Instruction::getPrev() {
    return prev;
}

void Instruction::remove() {
    this->getNext()->setPrev(this->getPrev());
    this->getPrev()->setNext(this->getNext());
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    int opdcnt = operands.size();
    for (int i = 1; i < opdcnt; i++) {
        operands[i]->removeUse(this);
    }
}

BinaryInstruction::BinaryInstruction(unsigned opcode,
                                     Operand* dst,
                                     Operand* src1,
                                     Operand* src2,
                                     BasicBlock* insert_bb)
    : Instruction(BINARY, insert_bb), dst(dst), src1(src1), src2(src2) {
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

void BinaryInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    } 
    if (operands[2] == old) {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void BinaryInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

BinaryInstruction::~BinaryInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

std::pair<int, int> BinaryInstruction::getLatticeValue(std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res, val1, val2;
    if (value.find(src1) == value.end())
        val1 = src1->getInitLatticeValue();
    else
        val1 = value[src1];
    if (value.find(src2) == value.end())
        val2 = src2->getInitLatticeValue();
    else
        val2 = value[src2];

    if (val1.first == -1 || val2.first == -1)
        res = {-1, 0};
    else if (val1.first == 0 && val2.first == 0)
    {
        res.first = 0;
        switch (opcode)
        {
        case ADD:
            res.second = val1.second + val2.second;
            break;
        case SUB:
            res.second = val1.second - val2.second;
            break;
        case MUL:
            res.second = val1.second * val2.second;
            break;
        case DIV:
            res.second = val1.second / val2.second;
            break;
        case MOD:
            res.second = val1.second % val2.second;
            break;
        case AND:
            res.second = val1.second & val2.second;
            break;
        case OR:
            res.second = val1.second | val2.second;
            break;
        default:
            break;
        }
    }
    else
        res.first = 1;
    return res;
}

void BinaryInstruction::output() const {
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode) {
        case ADD:
            if (type == "float") {
                op = "fadd";
            } else {
                op = "add";
            }
            break;
        case SUB:
            if (type == "float") {
                op = "fsub";
            } else {
                op = "sub";
            }
            break;
        case MUL:
            if (type == "float") {
                op = "fmul";
            } else {
                op = "mul";
            }
            break;
        case DIV:
            if (type == "float") {
                op = "fdiv";
            } else {
                op = "sdiv";
            }
            break;
        case MOD:
            op = "srem";
            break;
        default:
            break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(),
            type.c_str(), s2.c_str(), s3.c_str());
}

CmpInstruction::CmpInstruction(unsigned opcode,
                               Operand* dst,
                               Operand* src1,
                               Operand* src2,
                               BasicBlock* insert_bb)
    : Instruction(CMP, insert_bb), dst(dst), src1(src1), src2(src2) {
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

void CmpInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    } 
    if (operands[2] == old) {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void CmpInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

CmpInstruction::~CmpInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const {
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
    switch (opcode) {
        case E:
            op = "eq";
            break;
        case NE:
            op = "ne";
            break;
        case L:
            op = "slt";
            break;
        case LE:
            op = "sle";
            break;
        case G:
            op = "sgt";
            break;
        case GE:
            op = "sge";
            break;
        default:
            op = "";
            break;
    }

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(),
            type.c_str(), s2.c_str(), s3.c_str());
}

std::pair<int, int> CmpInstruction::getLatticeValue(std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res, val1, val2;
    if (value.find(src1) == value.end())
        val1 = src1->getInitLatticeValue();
    else
        val1 = value[src1];
    if (value.find(src2) == value.end())
        val2 = src2->getInitLatticeValue();
    else
        val2 = value[src2];
    if (val1.first == -1 || val2.first == -1){
        res = {-1, 0};
    }
    else if (val1.first == 0 && val2.first == 0)
    {
        res.first = 0;
        switch (opcode)
        {
        case E:
            res.second = val1.second == val2.second;
            break;
        case NE:
            res.second = val1.second != val2.second;
            break;
        case L:
            res.second = val1.second < val2.second;
            break;
        case GE:
            res.second = val1.second >= val2.second;
            break;
        case G:
            res.second = val1.second > val2.second;
            break;
        case LE:
            res.second = val1.second <= val2.second;
            break;
        default:
            break;
        }
    }
    else{
        res.first = 1;
    }
    return res;
}

UncondBrInstruction::UncondBrInstruction(BasicBlock* to, BasicBlock* insert_bb)
    : Instruction(UNCOND, insert_bb) {
    branch = to;
}

void UncondBrInstruction::output() const {
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock* bb) {
    branch = bb;
}

BasicBlock* UncondBrInstruction::getBranch() {
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock* true_branch,
                                     BasicBlock* false_branch,
                                     Operand* cond,
                                     BasicBlock* insert_bb)
    : Instruction(COND, insert_bb) {
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
    originTrue = originFalse = nullptr;
}

void CondBrInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[0] == old) {
        operands[0]->removeUse(this);
        operands[0] = new_;
        new_->addUse(this);
    }
}

CondBrInstruction::~CondBrInstruction() {
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const {
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(),
            cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock* bb) {
    false_branch = bb;
    if (!originFalse)
        originFalse = false_branch;
}

BasicBlock* CondBrInstruction::getFalseBranch() {
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock* bb) {
    true_branch = bb;
    if (!originTrue)
        originTrue = true_branch;
}

BasicBlock* CondBrInstruction::getTrueBranch() {
    return true_branch;
}

RetInstruction::RetInstruction(Operand* src, BasicBlock* insert_bb)
    : Instruction(RET, insert_bb) {
    if (src != nullptr) {
        operands.push_back(src);
        src->addUse(this);
    }
}

void RetInstruction::replaceDef(Operand* new_) {
    if (operands.size()) {
        operands[0]->removeDef(this);
        operands[0] = new_;
        new_->setDef(this);
    }
}

void RetInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands.size() && operands[0] == old) {
        operands[0]->removeUse(this);
        operands[0] = new_;
        new_->addUse(this);
    }
}

RetInstruction::~RetInstruction() {
    if (!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const {
    if (operands.empty()) {
        fprintf(yyout, "  ret void\n");
    } else {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}

AllocaInstruction::AllocaInstruction(Operand* dst,
                                     SymbolEntry* se,
                                     BasicBlock* insert_bb)
    : Instruction(ALLOCA, insert_bb) {
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

void AllocaInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

AllocaInstruction::~AllocaInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const {
    std::string dst, type;
    dst = operands[0]->toStr();
    if (se->getType()->isInt() || se->getType()->isFloat()) {
        type = se->getType()->toStr();
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(),
                type.c_str());
    } else if (se->getType()->isArray()) {
        type = se->getType()->toStr();
        // type = operands[0]->getSymbolEntry()->getType()->toStr();
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(),
                type.c_str());
    }
}

LoadInstruction::LoadInstruction(Operand* dst,
                                 Operand* src_addr,
                                 BasicBlock* insert_bb)
    : Instruction(LOAD, insert_bb), dst(dst), src_addr(src_addr) {
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

void LoadInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

void LoadInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

LoadInstruction::~LoadInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const {
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(),
            dst_type.c_str(), src_type.c_str(), src.c_str());
}

StoreInstruction::StoreInstruction(Operand* dst_addr,
                                   Operand* src,
                                   BasicBlock* insert_bb)
    : Instruction(STORE, insert_bb) {
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

void StoreInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[0] == old) {
        operands[0]->removeUse(this);
        operands[0] = new_;
        new_->addUse(this);
    } 
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

StoreInstruction::~StoreInstruction() {
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const {
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(),
            src.c_str(), dst_type.c_str(), dst.c_str());
}

MachineOperand* Instruction::genMachineOperand(Operand* ope) {
    auto se = ope->getEntry();
    MachineOperand* mope = nullptr;
    if (se->isConstant()) {
        mope = new MachineOperand(
            MachineOperand::IMM,
            (int)dynamic_cast<ConstantSymbolEntry*>(se)->getValue());
    } else if (se->isTemporary()) {
        mope = new MachineOperand(
            MachineOperand::VREG,
            dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());
    } else if (se->isVariable()) {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        if (id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().c_str());
        else if (id_se->isParam()) {
            // TODO: 这样分配的是虚拟寄存器 能对应到r0-r3嘛
            //  r4之后的参数需要一条load 哪里加 怎么判断是r4之后的参数
            auto no = id_se->getParamNo();
            if (no < 4)
                mope = new MachineOperand(MachineOperand::REG,
                                          id_se->getParamNo());
            else {
                // 用r3代表一下
                mope = new MachineOperand(MachineOperand::REG, 3);
                mope->setParam();
                mope->setParamNo(no);
            }
        } else
            exit(0);
    }
    return mope;
}

MachineOperand* Instruction::genMachineFloatOperand(Operand* ope) {
    auto se = ope->getEntry();
    if (!se->getType()->isFloat()) {
        // error
        return genMachineOperand(ope);
    }
    MachineOperand* mope = nullptr;
    if (se->isConstant()) {
        mope = new MachineOperand(
            MachineOperand::IMM,
            (float)dynamic_cast<ConstantSymbolEntry*>(se)->getValue());
        // TODO
    } else if (se->isTemporary()) {
        mope = new MachineOperand(
            MachineOperand::VREG,
            dynamic_cast<TemporarySymbolEntry*>(se)->getLabel(), true);
    } else if (se->isVariable()) {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        if (id_se->isGlobal()) {
            mope = new MachineOperand(id_se->toStr().c_str());
        } else if (id_se->isParam()) {
            if (id_se->getParamNo() < 4) {
                mope = new MachineOperand(MachineOperand::REG,
                                          id_se->getParamNo() + 16, true);
            } else {
                mope = new MachineOperand(MachineOperand::REG, 19, true);
            }
        }
    }
    return mope;
}

MachineOperand* Instruction::genMachineReg(int reg) {
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand* Instruction::genMachineFReg(int freg) {
    return new MachineOperand(MachineOperand::REG, freg + 16);
}

MachineOperand* Instruction::genMachineVReg(bool fpu) {
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel(),
                              fpu);
}

MachineOperand* Instruction::genMachineImm(int val) {
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand* Instruction::genMachineLabel(int block_no) {
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder) {
    /* HINT:
     * Allocate stack space for local variabel
     * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    int size = se->getType()->getSize() / 8;
    if (size < 0)
        size = 4;
    int offset = cur_func->AllocSpace(size);
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())
        ->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    // Load global operand
    if (operands[1]->getEntry()->isVariable() &&
        dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())
            ->isGlobal()) {
        if (operands[0]->getType()->isFloat()) {
            auto dst = genMachineFloatOperand(operands[0]);
            auto internal_reg1 = genMachineVReg();
            auto internal_reg2 = new MachineOperand(*internal_reg1);
            auto src = genMachineOperand(operands[1]);
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg1, src);
            cur_block->InsertInst(cur_inst);
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::VLDR,
                                            dst, internal_reg2);
            cur_block->InsertInst(cur_inst);
        } else {
            auto dst = genMachineOperand(operands[0]);
            auto internal_reg1 = genMachineVReg();
            auto internal_reg2 = new MachineOperand(*internal_reg1);
            auto src = genMachineOperand(operands[1]);
            // example: load r0, addr_a
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg1, src);
            cur_block->InsertInst(cur_inst);
            // example: load r1, [r0]
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            dst, internal_reg2);
            cur_block->InsertInst(cur_inst);
        }
    }
    // Load local operand
    else if (operands[1]->getEntry()->isTemporary() && operands[1]->getDef() &&
             operands[1]->getDef()->isAlloc()) {
        // example: load r1, [r0, #4]
        if (operands[0]->getType()->isFloat()) {
            auto dst = genMachineFloatOperand(operands[0]);
            auto src1 = genMachineReg(11);
            int off =
                dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())
                    ->getOffset();
            auto src2 = genMachineImm(off);
            if (off > 255 || off < -255) {
                auto operand = genMachineVReg(true);
                cur_block->InsertInst((new LoadMInstruction(
                    cur_block, LoadMInstruction::VLDR, operand, src2)));
                src2 = operand;
            }
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::VLDR,
                                            dst, src1, src2);
            cur_block->InsertInst(cur_inst);  // TODO
        } else {
            auto dst = genMachineOperand(operands[0]);
            auto src1 = genMachineReg(11);
            int off =
                dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())
                    ->getOffset();
            auto src2 = genMachineImm(off);
            if (off > 255 || off < -255) {
                auto operand = genMachineVReg();
                cur_block->InsertInst((new LoadMInstruction(
                    cur_block, LoadMInstruction::LDR, operand, src2)));
                src2 = operand;
            }
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            dst, src1, src2);
            cur_block->InsertInst(cur_inst);
        }
    } else {  // Load operand from temporary variable
        // example: load r1, [r0]
        MachineOperand* dst = nullptr;
        MachineOperand* src = nullptr;
        if (operands[0]->getType()->isFloat()) {
            dst = genMachineFloatOperand(operands[0]);
        } else {
            dst = genMachineOperand(operands[0]);
        }
        if (operands[1]->getType()->isFloat()) {
            src = genMachineFloatOperand(operands[1]);
        } else {
            src = genMachineOperand(operands[1]);
        }
        if (operands[0]->getType()->isFloat() ||
            operands[1]->getType()->isFloat()) {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::VLDR,
                                            dst, src);
            cur_block->InsertInst(cur_inst);
        } else {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            dst, src);
            cur_block->InsertInst(cur_inst);
        }
    }
}

void StoreInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;

    bool dst_float = operands[0]->getType()->isFloat();
    bool src_float = operands[1]->getType()->isFloat();

    MachineOperand* dst = nullptr;
    MachineOperand* src = nullptr;

    if (dst_float) {
        dst = genMachineFloatOperand(operands[0]);

    } else {
        dst = genMachineOperand(operands[0]);
    }

    if (src_float) {
        src = genMachineFloatOperand(operands[1]);

    } else {
        src = genMachineOperand(operands[1]);
    }

    // store immediate
    if (operands[1]->getEntry()->isConstant()) {
        auto dst1 = genMachineVReg(src_float);
        if (src_float) {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::VLDR,
                                            dst1, src);
        } else {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            dst1, src);
        }
        cur_block->InsertInst(cur_inst);
        // src = dst1;
        src = new MachineOperand(*dst1);
    }
    // store to local
    if (operands[0]->getEntry()->isTemporary() && operands[0]->getDef() &&
        operands[0]->getDef()->isAlloc()) {
        auto src1 = genMachineReg(11);
        int off = dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())
                      ->getOffset();
        auto src2 = genMachineImm(off);
        if (off > 255 || off < -255) {
            auto operand = genMachineVReg();
            cur_block->InsertInst((new LoadMInstruction(
                cur_block, LoadMInstruction::LDR, operand, src2)));
            src2 = operand;
        }
        if (src_float) {
            cur_inst = new StoreMInstruction(cur_block, StoreMInstruction::VSTR,
                                             src, src1, src2);
        } else {
            cur_inst = new StoreMInstruction(cur_block, StoreMInstruction::STR,
                                             src, src1, src2);
        }
        cur_block->InsertInst(cur_inst);
    }
    // store to global
    else if (operands[0]->getEntry()->isVariable() &&
             dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getEntry())
                 ->isGlobal()) {
        auto internal_reg1 = genMachineVReg(src_float);
        // example: load r0, addr_a
        if (src_float) {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::VLDR,
                                            internal_reg1, dst);
            cur_block->InsertInst(cur_inst);
            cur_inst = new StoreMInstruction(cur_block, StoreMInstruction::VSTR,
                                             src, internal_reg1);
            cur_block->InsertInst(cur_inst);
        } else {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg1, dst);
            cur_block->InsertInst(cur_inst);
            // example: store r1, [r0]
            cur_inst = new StoreMInstruction(cur_block, StoreMInstruction::STR,
                                             src, internal_reg1);
            cur_block->InsertInst(cur_inst);
        }
    }
    // store to pointer
    else if (operands[0]->getType()->isPtr()) {
        if (src_float) {
            cur_inst = new StoreMInstruction(cur_block, StoreMInstruction::VSTR,
                                             src, dst);
            cur_block->InsertInst(cur_inst);
        } else {
            cur_inst = new StoreMInstruction(cur_block, StoreMInstruction::STR,
                                             src, dst);
            cur_block->InsertInst(cur_inst);
        }
    }
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder) {
    // complete other instructions
    auto cur_block = builder->getBlock();
    /* HINT:
     * The source operands of ADD instruction in ir code both can be immediate
     * num. However, it's not allowed in assembly code. So you need to insert
     * LOAD/MOV instrucrion to load immediate num into register. As to other
     * instructions, such as MUL, CMP, you need to deal with this situation,
     * too.*/

    if (operands[0]->getType()->isFloat()) {
        auto dst = genMachineFloatOperand(operands[0]);
        auto src1 = genMachineFloatOperand(operands[1]);
        auto src2 = genMachineFloatOperand(operands[2]);
        MachineInstruction* cur_inst = nullptr;
        if (src1->isImm()) {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            auto tmp_reg = genMachineVReg(true);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                           tmp_reg, internal_reg);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*tmp_reg);
        }
        if (src2->isImm()) {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            auto tmp_reg = genMachineVReg(true);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                           tmp_reg, internal_reg);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*tmp_reg);
        }

        switch (opcode) {
            case ADD:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::VADD, dst, src1, src2);
                break;
            case SUB:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::VSUB, dst, src1, src2);
                break;
            case AND:
                // TODO
                break;
            case OR:
                // TODO
                break;
            case MUL:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::VMUL, dst, src1, src2);
                break;
            case DIV:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::VDIV, dst, src1, src2);
                break;
            case MOD:
                // error
                break;
            default:
                break;
        }
        cur_block->InsertInst(cur_inst);

    } else {
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineOperand(operands[1]);
        auto src2 = genMachineOperand(operands[2]);
        MachineInstruction* cur_inst = nullptr;
        if (src1->isImm() && src2->isImm() && src2->getVal() == 0 &&
            opcode == ADD) {
            if (!(src1->getVal() < 256 && src1->getVal() > -255)) {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(
                    cur_block, LoadMInstruction::LDR, internal_reg, src1);
                cur_block->InsertInst(cur_inst);
                src1 = new MachineOperand(*internal_reg);
            }
            cur_inst =
                new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src1);
            cur_block->InsertInst(cur_inst);
            return;
        }
        if (src1->isImm()) {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*internal_reg);
        }
        // 合法立即数的判定太复杂 简单判定255以上均load
        // 这里应该也不需要考虑负数
        if (src2->isImm()) {
            if ((opcode <= BinaryInstruction::OR &&
                 ((ConstantSymbolEntry*)(operands[2]->getEntry()))->getValue() >
                     255) ||
                opcode >= BinaryInstruction::MUL) {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(
                    cur_block, LoadMInstruction::LDR, internal_reg, src2);
                cur_block->InsertInst(cur_inst);
                src2 = new MachineOperand(*internal_reg);
            }
        }
        switch (opcode) {
            case ADD:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::ADD, dst, src1, src2);
                break;
            case SUB:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::SUB, dst, src1, src2);
                break;
            case AND:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::AND, dst, src1, src2);
                break;
            case OR:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::OR, dst, src1, src2);
                break;
            case MUL:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::MUL, dst, src1, src2);
                break;
            case DIV:
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::DIV, dst, src1, src2);
                break;
            case MOD: {
                // c = a % b
                // c = a / b
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::DIV, dst, src1, src2);
                MachineOperand* dst1 = new MachineOperand(*dst);
                src1 = new MachineOperand(*src1);
                src2 = new MachineOperand(*src2);
                cur_block->InsertInst(cur_inst);
                // c = c * b
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::MUL, dst1, dst, src2);
                cur_block->InsertInst(cur_inst);
                dst = new MachineOperand(*dst1);
                // c = a - c
                cur_inst = new BinaryMInstruction(
                    cur_block, BinaryMInstruction::SUB, dst, src1, dst1);
                break;
            }
            default:
                break;
        }
        cur_block->InsertInst(cur_inst);
    }
}

void CmpInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();

    if (operands[1]->getType()->isFloat()) {
        auto src1 = genMachineFloatOperand(operands[1]);
        auto src2 = genMachineFloatOperand(operands[2]);
        MachineInstruction* cur_inst = nullptr;
        if (src1->isImm()) {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            auto tmp_reg = genMachineVReg(true);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                           tmp_reg, internal_reg);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*tmp_reg);
        }
        if (src2->isImm()) {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            auto tmp_reg = genMachineVReg(true);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                           tmp_reg, internal_reg);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*tmp_reg);
        }
        cur_inst = new CmpMInstruction(cur_block, CmpMInstruction::VCMP, src1,
                                       src2, opcode);
        cur_block->InsertInst(cur_inst);
        cur_inst = new VmrsMInstruction(cur_block);
        cur_block->InsertInst(cur_inst);

        if (opcode >= CmpInstruction::L && opcode <= CmpInstruction::GE) {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           trueOperand, opcode);
            cur_block->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           falseOperand, 7 - opcode);
            cur_block->InsertInst(cur_inst);
        } else if (opcode == CmpInstruction::E) {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           trueOperand, E);
            cur_block->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           falseOperand, NE);
            cur_block->InsertInst(cur_inst);

        } else if (opcode == CmpInstruction::NE) {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           trueOperand, NE);
            cur_block->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           falseOperand, E);
            cur_block->InsertInst(cur_inst);
        }

    } else {
        auto src1 = genMachineOperand(operands[1]);
        auto src2 = genMachineOperand(operands[2]);
        MachineInstruction* cur_inst = nullptr;
        if (src1->isImm()) {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*internal_reg);
        }
        if (src2->isImm() &&
            ((ConstantSymbolEntry*)(operands[2]->getEntry()))->getValue() >
                255) {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*internal_reg);
        }
        cur_inst = new CmpMInstruction(cur_block, CmpMInstruction::CMP, src1,
                                       src2, opcode);
        cur_block->InsertInst(cur_inst);
        if (opcode >= CmpInstruction::L && opcode <= CmpInstruction::GE) {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           trueOperand, opcode);
            cur_block->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           falseOperand, 7 - opcode);
            cur_block->InsertInst(cur_inst);
        }
    }
}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    std::stringstream s;
    s << ".L" << branch->getNo();
    MachineOperand* dst = new MachineOperand(s.str());
    auto cur_inst =
        new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}

void CondBrInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    std::stringstream s;
    s << ".L" << true_branch->getNo();
    MachineOperand* dst = new MachineOperand(s.str());
    auto cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B,
                                           dst, cur_block->getCmpCond());
    cur_block->InsertInst(cur_inst);
    s.str("");
    s << ".L" << false_branch->getNo();
    dst = new MachineOperand(s.str());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}

void RetInstruction::genMachineCode(AsmBuilder* builder) {
    /* HINT:
     * 1. Generate mov instruction to save return value in r0
     * 2. Restore callee saved registers and sp, fp
     * 3. Generate bx instruction */
    auto cur_block = builder->getBlock();
    if (!operands.empty()) {
        MachineOperand* dst;
        MachineOperand* src;
        MachineInstruction* cur_inst;

        if (operands[0]->getType()->isFloat()) {
            dst = new MachineOperand(MachineOperand::REG, 16, true);
            src = genMachineFloatOperand(operands[0]);
            if (src->isImm()) {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(
                    cur_block, LoadMInstruction::LDR, internal_reg, src);
                cur_block->InsertInst(cur_inst);
                src = internal_reg;
            }
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                           dst, src);  // TODO: movw movt
        } else {
            dst = new MachineOperand(MachineOperand::REG, 0);
            src = genMachineOperand(operands[0]);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                           src);  // TODO: movw movt
        }
        cur_block->InsertInst(cur_inst);
    }
    auto cur_func = builder->getFunction();
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto size =
        new MachineOperand(MachineOperand::IMM, cur_func->AllocSpace(0));
    auto cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,
                                           sp, sp, size);
    cur_block->InsertInst(cur_inst);
    // auto fp = new MachineOperand(MachineOperand::REG, 11);
    // auto cur_inst1 = new StackMInstruction(cur_block, StackMInstruction::POP,
    //                                       cur_func->getSavedRegs(), fp);
    // cur_block->InsertInst(cur_inst1);
    auto lr = new MachineOperand(MachineOperand::REG, 14);
    auto cur_inst2 =
        new BranchMInstruction(cur_block, BranchMInstruction::BX, lr);
    cur_block->InsertInst(cur_inst2);
}

CallInstruction::CallInstruction(Operand* dst,
                                 SymbolEntry* func,
                                 std::vector<Operand*> params,
                                 BasicBlock* insert_bb)
    : Instruction(CALL, insert_bb), func(func), dst(dst) {
    operands.push_back(dst);
    if (dst)
        dst->setDef(this);
    for (auto param : params) {
        operands.push_back(param);
        param->addUse(this);
    }
    insert_bb->getParent()->setHasCall();
    IdentifierSymbolEntry* funcSE = (IdentifierSymbolEntry*)func;
    if (!funcSE->isSysy() && funcSE->getName() != "llvm.memset.p0.i32")
        funcSE->getFunction()->addPred(this);
}

void CallInstruction::addPred() {
    IdentifierSymbolEntry* funcSE = (IdentifierSymbolEntry*)func;
    if (!funcSE->isSysy() && funcSE->getName() != "llvm.memset.p0.i32")
        funcSE->getFunction()->addPred(this);
}

void CallInstruction::replaceDef(Operand* new_) {
    if (dst) {
        operands[0]->removeDef(this);
        operands[0] = new_;
        new_->setDef(this);
        dst = new_;
    }
}

void CallInstruction::replaceUse(Operand* old, Operand* new_) {
    for (int i = 1; i < (int)operands.size(); i++)
        if (operands[i] == old) {
            operands[i]->removeUse(this);
            operands[i] = new_;
            new_->addUse(this);
        }
}

void CallInstruction::output() const {
    fprintf(yyout, "  ");
    if (operands[0])
        fprintf(yyout, "%s = ", operands[0]->toStr().c_str());
    FunctionType* type = (FunctionType*)(func->getType());
    fprintf(yyout, "call %s %s(", type->getRetType()->toStr().c_str(),
            func->toStr().c_str());
    for (long unsigned int i = 1; i < operands.size(); i++) {
        if (i != 1)
            fprintf(yyout, ", ");
        fprintf(yyout, "%s %s", operands[i]->getType()->toStr().c_str(),
                operands[i]->toStr().c_str());
    }
    fprintf(yyout, ")\n");
}

CallInstruction::~CallInstruction() {
    if (operands[0]) {
        operands[0]->setDef(nullptr);
        if (operands[0]->usersNum() == 0)
            delete operands[0];
    }
    for (long unsigned int i = 1; i < operands.size(); i++)
        operands[i]->removeUse(this);
}

ZextInstruction::ZextInstruction(Operand* dst,
                                 Operand* src,
                                 BasicBlock* insert_bb)
    : Instruction(ZEXT, insert_bb) {
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void ZextInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

void ZextInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

void ZextInstruction::output() const {
    Operand* dst = operands[0];
    Operand* src = operands[1];
    fprintf(yyout, "  %s = zext %s %s to i32\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str());
}

ZextInstruction::~ZextInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

XorInstruction::XorInstruction(Operand* dst,
                               Operand* src,
                               BasicBlock* insert_bb)
    : Instruction(XOR, insert_bb) {
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void XorInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

void XorInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

void XorInstruction::output() const {
    Operand* dst = operands[0];
    Operand* src = operands[1];
    fprintf(yyout, "  %s = xor %s %s, true\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str());
}

XorInstruction::~XorInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

GepInstruction::GepInstruction(Operand* dst,
                               Operand* arr,
                               Operand* idx,
                               BasicBlock* insert_bb,
                               bool paramFirst)
    : Instruction(GEP, insert_bb), paramFirst(paramFirst) {
    operands.push_back(dst);
    operands.push_back(arr);
    operands.push_back(idx);
    dst->setDef(this);
    arr->addUse(this);
    idx->addUse(this);
    first = false;
    init = nullptr;
    last = false;
    noAsm = false;
}

void GepInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

void GepInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    } else if (operands[2] == old) {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void GepInstruction::output() const {
    Operand* dst = operands[0];
    Operand* arr = operands[1];
    Operand* idx = operands[2];
    std::string arrType = arr->getType()->toStr();
    // Type* type = ((PointerType*)(arr->getType()))->getType();
    // ArrayType* type1 = (ArrayType*)(((ArrayType*)type)->getArrayType());
    // if (type->isInt() || (type1 && type1->getLength() == -1))
    if (paramFirst)
        fprintf(yyout, "  %s = getelementptr inbounds %s, %s %s, i32 %s\n",
                dst->toStr().c_str(),
                arrType.substr(0, arrType.size() - 1).c_str(), arrType.c_str(),
                arr->toStr().c_str(), idx->toStr().c_str());
    else
        fprintf(
            yyout, "  %s = getelementptr inbounds %s, %s %s, i32 0, i32 %s\n",
            dst->toStr().c_str(), arrType.substr(0, arrType.size() - 1).c_str(),
            arrType.c_str(), arr->toStr().c_str(), idx->toStr().c_str());
}

GepInstruction::~GepInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

FptosiInstruction::FptosiInstruction(Operand* dst,
                                     Operand* src,
                                     BasicBlock* insert_bb)
    : Instruction(FPTOSI, insert_bb), dst(dst), src(src) {
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void FptosiInstruction::output() const {
    Operand* dst = operands[0];
    Operand* src = operands[1];
    fprintf(yyout, "  %s = fptosi %s %s to %s\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str(),
            dst->getType()->toStr().c_str());
}

FptosiInstruction::~FptosiInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

SitofpInstruction::SitofpInstruction(Operand* dst,
                                     Operand* src,
                                     BasicBlock* insert_bb)
    : Instruction(FPTOSI, insert_bb), dst(dst), src(src) {
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void SitofpInstruction::output() const {
    Operand* dst = operands[0];
    Operand* src = operands[1];
    fprintf(yyout, "  %s = sitofp %s %s to %s\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str(),
            dst->getType()->toStr().c_str());
}

SitofpInstruction::~SitofpInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void CallInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    MachineOperand* operand;  //, *num;
    MachineInstruction* cur_inst;
    // auto fp = new MachineOperand(MachineOperand::REG, 11);
    // TODO
    size_t idx;
    auto funcSE = (IdentifierSymbolEntry*)func;
    if (funcSE->getName() == "llvm.memset.p0.i32") {
        auto r0 = genMachineReg(0);
        auto r1 = genMachineReg(1);
        auto r2 = genMachineReg(2);
        auto int8Ptr = operands[1];
        auto bitcast = (BitcastInstruction*)(int8Ptr->getDef());
        auto arraySE =
            (TemporarySymbolEntry*)(bitcast->getUse()[0]->getEntry());
        int offset = arraySE->getOffset();
        operand = genMachineVReg();
        auto fp = genMachineReg(11);
        if (offset > -255 && offset < 255) {
            cur_block->InsertInst(
                new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, r0,
                                       fp, genMachineImm(offset)));
        } else {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            operand, genMachineImm(offset));
            operand = new MachineOperand(*operand);
            cur_block->InsertInst(cur_inst);
            cur_block->InsertInst(new BinaryMInstruction(
                cur_block, BinaryMInstruction::ADD, r0, fp, operand));
        }
        cur_block->InsertInst(new MovMInstruction(
            cur_block, MovMInstruction::MOV, r1, genMachineImm(0)));
        auto len = genMachineOperand(operands[3]);
        if (len->isImm() && len->getVal() > 255) {
            operand = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            operand, len);
            operand = new MachineOperand(*operand);
            cur_block->InsertInst(cur_inst);
        } else
            operand = len;
        cur_block->InsertInst(
            new MovMInstruction(cur_block, MovMInstruction::MOV, r2, operand));
        cur_block->InsertInst(new BranchMInstruction(
            cur_block, BranchMInstruction::BL, new MachineOperand("@memset")));
        return;
    }

    int stk_cnt = 0;
    std::vector<MachineOperand*> vec;

    bool additional_push = false;  // for alignment
    int float_num = 0;
    int int_num = 0;
    for (size_t i = 1; i < operands.size(); i++) {
        if (operands[i]->getType()->isFloat()) {
            float_num++;
        } else {
            int_num++;
        }
    }

    int push_num = 0;
    if (float_num > 4) {
        push_num += float_num - 4;
    }
    if (int_num > 4) {
        push_num += int_num - 4;
    }

    if (push_num % 2 != 0) {
        additional_push = true;
    }

    int gpreg_cnt = 1;
    for (idx = 1; idx < operands.size(); idx++) {
        if (gpreg_cnt == 5)
            break;
        if (operands[idx]->getType()->isFloat()) {
            continue;
        }
        operand = genMachineReg(gpreg_cnt - 1);
        auto src = genMachineOperand(operands[idx]);
        if (src->isImm() && src->getVal() > 255) {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            operand, src);
        } else {
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV,
                                           operand, src);
        }
        cur_block->InsertInst(cur_inst);
        gpreg_cnt++;
    }

    size_t int_idx = idx;

    int fpreg_cnt = 1;
    for (idx = 1; idx < operands.size(); idx++) {
        if (fpreg_cnt == 5)
            break;
        if (!operands[idx]->getType()->isFloat()) {
            continue;
        }
        operand = genMachineFReg(fpreg_cnt - 1);
        auto src = genMachineFloatOperand(operands[idx]);
        if (src->isImm()) {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::VLDR,
                                            operand, src);
        } else {
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                           operand, src);
        }
        cur_block->InsertInst(cur_inst);
        fpreg_cnt++;
    }

    size_t float_idx = idx;

    if (additional_push) {
        cur_inst = new StackMInstruction(cur_block, StackMInstruction::PUSH,
                                         vec, genMachineReg(0));
        cur_block->InsertInst(cur_inst);
        stk_cnt++;
    }

    idx = std::min(float_idx, int_idx);

    for (size_t i = operands.size() - 1; i >= idx; i--) {
        if (operands[i]->getType()->isFloat() && i >= float_idx) {
            operand = genMachineFloatOperand(operands[i]);
            if (operand->isImm()) {
                auto dst = genMachineVReg(true);
                cur_inst = new LoadMInstruction(
                    cur_block, LoadMInstruction::VLDR, dst, operand);  // TODO
                cur_block->InsertInst(cur_inst);
                operand = new MachineOperand(*dst);
            }
            cur_inst = new StackMInstruction(
                cur_block, StackMInstruction::VPUSH, vec, operand);
            cur_block->InsertInst(cur_inst);
            stk_cnt++;
        } else if (!operands[i]->getType()->isFloat() && i >= int_idx) {
            operand = genMachineOperand(operands[i]);
            if (operand->isImm()) {
                auto dst = genMachineVReg();
                if (operand->getVal() < 256) {
                    cur_inst = new MovMInstruction(
                        cur_block, MovMInstruction::MOV, dst, operand);
                } else {
                    cur_inst = new LoadMInstruction(
                        cur_block, LoadMInstruction::LDR, dst, operand);
                }
                cur_block->InsertInst(cur_inst);
                operand = new MachineOperand(*dst);
            }
            cur_inst = new StackMInstruction(cur_block, StackMInstruction::PUSH,
                                             vec, operand);
            cur_block->InsertInst(cur_inst);
            stk_cnt++;
        }
    }

    auto label = new MachineOperand(func->toStr().c_str());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, label);
    cur_block->InsertInst(cur_inst);
    if ((gpreg_cnt >= 5 || fpreg_cnt >= 5) && stk_cnt != 0) {
        auto off = genMachineImm(stk_cnt * 4);
        auto sp = new MachineOperand(MachineOperand::REG, 13);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,
                                          sp, sp, off);
        cur_block->InsertInst(cur_inst);
    }
    if (dst) {
        if (dst->getType()->isFloat()) {
            operand = genMachineFloatOperand(dst);
            auto s0 = new MachineOperand(MachineOperand::REG, 16, true);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                           operand, s0);
            cur_block->InsertInst(cur_inst);
        } else {
            operand = genMachineOperand(dst);
            auto r0 = new MachineOperand(MachineOperand::REG, 0);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV,
                                           operand, r0);
            cur_block->InsertInst(cur_inst);
        }
    }
}

void ZextInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto cur_inst =
        new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
    cur_block->InsertInst(cur_inst);
}

void XorInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto trueOperand = genMachineImm(1);
    auto falseOperand = genMachineImm(0);
    auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                        trueOperand, MachineInstruction::EQ);
    cur_block->InsertInst(cur_inst);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,
                                   falseOperand, MachineInstruction::NE);
    cur_block->InsertInst(cur_inst);
}

void GepInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst;
    auto dst = genMachineOperand(operands[0]);
    auto idx = genMachineOperand(operands[2]);
    if (init) {
        if (last) {
            auto base = genMachineOperand(init);
            MachineOperand* imm = genMachineImm(off + 4);
            int off = this->off + 4;
            if (off > 255) {
                MachineOperand* temp = genMachineVReg();
                cur_block->InsertInst(new LoadMInstruction(
                    cur_block, LoadMInstruction::LDR, temp, imm));
                imm = temp;
            }
            cur_inst = new BinaryMInstruction(
                cur_block, BinaryMInstruction::ADD, dst, base, imm);
            cur_block->InsertInst(cur_inst);
        } else {
            noAsm = true;
        }
        return;
    }
    MachineOperand* base = nullptr;
    int size;
    auto idx1 = genMachineVReg();
    if (idx->isImm()) {
        if (idx->getVal() < 255) {
            cur_inst =
                new MovMInstruction(cur_block, MovMInstruction::MOV, idx1, idx);
        } else {
            cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                            idx1, idx);
        }
        idx = new MachineOperand(*idx1);
        cur_block->InsertInst(cur_inst);
    }
    if (paramFirst) {
        size =
            ((PointerType*)(operands[1]->getType()))->getType()->getSize() / 8;
    } else {
        if (first) {
            base = genMachineVReg();
            if (operands[1]->getEntry()->isVariable() &&
                ((IdentifierSymbolEntry*)(operands[1]->getEntry()))
                    ->isGlobal()) {
                auto src = genMachineOperand(operands[1]);
                cur_inst = new LoadMInstruction(
                    cur_block, LoadMInstruction::LDR, base, src);
            } else {
                int offset = ((TemporarySymbolEntry*)(operands[1]->getEntry()))
                                 ->getOffset();
                if (offset > -255 && offset < 255) {
                    cur_inst =
                        new MovMInstruction(cur_block, MovMInstruction::MOV,
                                            base, genMachineImm(offset));
                } else {
                    cur_inst =
                        new LoadMInstruction(cur_block, LoadMInstruction::LDR,
                                             base, genMachineImm(offset));
                }
            }
            cur_block->InsertInst(cur_inst);
        }

        ArrayType* type =
            (ArrayType*)(((PointerType*)(operands[1]->getType()))->getType());
        Type* elementType = type->getElementType();
        size = elementType->getSize() / 8;
    }
    auto size1 = genMachineVReg();
    if (size > -255 && size < 255) {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, size1,
                                       genMachineImm(size));
    } else {
        cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR, size1,
                                        genMachineImm(size));
    }
    cur_block->InsertInst(cur_inst);
    size1 = new MachineOperand(*size1);
    auto off = genMachineVReg();
    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, off,
                                      idx, size1);
    off = new MachineOperand(*off);
    cur_block->InsertInst(cur_inst);
    if (paramFirst || !first) {
        auto arr = genMachineOperand(operands[1]);
        auto in = operands[1]->getDef();
        if (in->isGep()) {
            auto gep = (GepInstruction*)in;
            if (gep->hasNoAsm()) {
                gep->setInit(nullptr, 0);
                gep->genMachineCode(builder);
            }
        }
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,
                                          dst, arr, off);
        cur_block->InsertInst(cur_inst);
    } else {
        auto addr = genMachineVReg();
        auto base1 = new MachineOperand(*base);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,
                                          addr, base1, off);
        cur_block->InsertInst(cur_inst);
        addr = new MachineOperand(*addr);
        if (operands[1]->getEntry()->isVariable() &&
            ((IdentifierSymbolEntry*)(operands[1]->getEntry()))->isGlobal()) {
            cur_inst =
                new MovMInstruction(cur_block, MovMInstruction::MOV, dst, addr);
        } else {
            auto fp = genMachineReg(11);
            cur_inst = new BinaryMInstruction(
                cur_block, BinaryMInstruction::ADD, dst, fp, addr);
        }
        cur_block->InsertInst(cur_inst);
    }
}

PhiInstruction::PhiInstruction(Operand* dst, BasicBlock* insert_bb)
    : Instruction(PHI, insert_bb) {
    operands.push_back(dst);
    this->dst = dst;
    this->originDef = dst;
    dst->setDef(this);
}

PhiInstruction::~PhiInstruction() {
    dst->setDef(nullptr);
    if (dst->usersNum() == 0)
        delete dst;
    for (auto it : srcs)
        it.second->removeUse(this);
}

void PhiInstruction::cleanUse() {
    for (auto it : srcs)
        it.second->removeUse(this);
}

void PhiInstruction::output() const {
    fprintf(yyout, "  %s = phi %s", dst->toStr().c_str(),
            dst->getType()->toStr().c_str());
    bool first = true;
    for (auto it = srcs.begin(); it != srcs.end(); it++) {
        if (!first)
            fprintf(yyout, ", ");
        else
            first = false;
        fprintf(yyout, "[ %s , %%B%d ]", it->second->toStr().c_str(),
                it->first->getNo());
    }
    fprintf(yyout, "\n");
}

std::pair<int, int> PhiInstruction::getLatticeValue(std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res, tmp;
    res = {1, 0};
    /*
    for (auto i = srcs.begin(); i != srcs.end(); i++)
    {
        if (value.find((*i).second) == value.end())
            tmp = (*i).second->getInitLatticeValue();
        else
            tmp = value[(*i).second];
        if (res.first > tmp.first)
            res = tmp;
        else if (res.first == tmp.first)
        {
            if (res.first == 0)
            {
                if (res.second != tmp.second)
                    res = {-1, 0};
            }
        }
    }*/
    return res;
}

void PhiInstruction::addSrc(BasicBlock* block, Operand* src) {
    operands.push_back(src);
    srcs.insert(std::make_pair(block, src));
    src->addUse(this);
}

void PhiInstruction::replaceUse(Operand* old, Operand* new_) {
    for (auto& it : srcs) {
        if (it.second == old) {
            it.second->removeUse(this);
            it.second = new_;
            new_->addUse(this);
        }
    }
    for (auto it = operands.begin() + 1; it != operands.end(); it++)
        if (*it == old)
            *it = new_;
}

void PhiInstruction::replaceDef(Operand* new_) {
    dst->removeDef(this);
    dst = new_;
    new_->setDef(this);
}

void PhiInstruction::replaceOriginDef(Operand* new_) {
    this->originDef = new_;
}

void PhiInstruction::changeSrcBlock(
    std::map<BasicBlock*, std::vector<BasicBlock*>> changes) {
    std::map<Operand*, BasicBlock*> originBlocks;
    for (auto it : srcs)
        originBlocks[it.second] = it.first;
    bool flag;
    while (true) {
        flag = false;
        for (auto& it : srcs) {
            if (changes.find(it.first) != changes.end()) {
                auto vec = changes[it.first];
                auto src = srcs[it.first];
                for (auto b : vec) {
                    if (srcs.find(b) != srcs.end()) {
                        auto& phiBlocks = parent->getPhiBlocks();
                        auto iter = phiBlocks.find(b);
                        BasicBlock* b1;
                        if (iter != phiBlocks.end()) {
                            b1 = iter->second;
                        } else {
                            // 需要添加一个block
                            b1 = new BasicBlock(b->getParent());
                            phiBlocks.insert(std::make_pair(b, b1));
                            b->addSucc(b1);
                            b->removeSuccFromEnd(this->getParent());
                            auto i = (CondBrInstruction*)(b->rbegin());
                            auto nowSrc = srcs[b];
                            auto originBlock = originBlocks[nowSrc];
                            while (true) {
                                if (i->getOriginFalse() == originBlock) {
                                    i->setTrueBranch(b1);
                                    break;
                                } else if (i->getOriginTrue() == originBlock) {
                                    i->setFalseBranch(b1);
                                    break;
                                }
                                if (changes.count(originBlock))
                                    originBlock = changes[originBlock][0];
                            }
                            b1->addPred(b);
                            new UncondBrInstruction(this->getParent(), b1);
                            this->getParent()->removePredFromEnd(b);
                            this->getParent()->addPred(b1);
                            b1->addSucc(this->getParent());
                        }
                        addSrc(b1, src);
                    } else
                        addSrc(b, src);
                }
                auto operand = it.second;
                operand->removeUse(this);
                operands.erase(
                    find(operands.begin() + 1, operands.end(), operand));
                srcs.erase(it.first);
                flag = true;
                break;
            }
        }
        if (!flag)
            break;
    }
}

Operand* PhiInstruction::getSrc(BasicBlock* block) {
    if (srcs.find(block) != srcs.end())
        return srcs[block];
    return nullptr;
}
void FptosiInstruction::genMachineCode(AsmBuilder* builder) {
    // arm fplib abi:
    // http://users.ece.utexas.edu/~valvano/Volume1/uvision/DUI0378C_using_arm_libraries.pdf

    MachineInstruction* cur_inst;
    auto cur_block = builder->getBlock();

    auto src_operand = genMachineFloatOperand(src);
    auto dst_operand = genMachineOperand(dst);

    if (src_operand->isImm()) {
        auto tmp = genMachineVReg(true);
        cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::VLDR, tmp,
                                        src_operand);
        cur_block->InsertInst(cur_inst);
        src_operand = tmp;
    }

    cur_inst = new VcvtMInstruction(cur_block, VcvtMInstruction::F2S,
                                    src_operand, src_operand);
    cur_block->InsertInst(cur_inst);

    cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                   dst_operand, src_operand);

    cur_block->InsertInst(cur_inst);
}

void SitofpInstruction::genMachineCode(AsmBuilder* builder) {
    MachineInstruction* cur_inst;
    auto cur_block = builder->getBlock();

    auto src_operand = genMachineOperand(src);

    if (src_operand->isImm()) {
        auto tmp = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, LoadMInstruction::LDR, tmp,
                                        src_operand);
        cur_block->InsertInst(cur_inst);
        src_operand = tmp;
    }
    auto dst_operand = genMachineFloatOperand(dst);

    cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,
                                   dst_operand, src_operand);
    cur_block->InsertInst(cur_inst);

    cur_inst = new VcvtMInstruction(cur_block, VcvtMInstruction::S2F,
                                    dst_operand, dst_operand);
    cur_block->InsertInst(cur_inst);
}

BitcastInstruction::BitcastInstruction(Operand* dst,
                                       Operand* src,
                                       BasicBlock* insert_bb)
    : Instruction(BITCAST, insert_bb), dst(dst), src(src) {
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void BitcastInstruction::output() const {
    Operand* dst = operands[0];
    Operand* src = operands[1];
    fprintf(yyout, "  %s = bitcast %s %s to %s\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str(),
            dst->getType()->toStr().c_str());
}

BitcastInstruction::~BitcastInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void BitcastInstruction::genMachineCode(AsmBuilder* builder) {}

bool AllocaInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::ALLOCA);
    return true;
};

bool LoadInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::LOAD);
    auto node1 = operands[1]->getDef()->getNode();
    node->addChild(node1);
    return true;
}

std::pair<int, int> LoadInstruction::getLatticeValue(std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> l;
    if (value.find(src_addr) == value.end())
        l = src_addr->getInitLatticeValue();
    else
        l = value[src_addr];
    return l;
}


bool BinaryInstruction::genNode() {
    // add 0
    if (operands[2]->isZero()) {
        auto se = operands[1]->getEntry();
        if (se->isConstant()) {
            int val = ((ConstantSymbolEntry*)se)->getValue();
            if (opcode == AND || opcode == MUL)
                val = 0;
            node = new SSAGraphNode(val);
            return true;
        } else {
            auto def = operands[1]->getDef();
            node = def->getNode();
            return true;
        }
    }
    auto se1 = operands[1]->getEntry();
    auto se2 = operands[2]->getEntry();
    // const +/-/... const
    if (se1->isConstant() && se2->isConstant()) {
        int val1 = ((ConstantSymbolEntry*)se1)->getValue();
        int val2 = ((ConstantSymbolEntry*)se2)->getValue();
        int val;
        switch (opcode) {
            case SUB:
                val = val1 - val2;
                break;
            case ADD:
                val = val1 + val2;
                break;
            case AND:
                val = val1 && val2;
                break;
            case OR:
                val = val1 || val2;
                break;
            case MUL:
                val = val1 * val2;
                break;
            case DIV:
                val = val1 / val2;
                break;
            case MOD:
                val = val1 % val2;
                break;
        }
        node = new SSAGraphNode(val);
        return true;
    }
    // 1 temp 1 const
    node = new SSAGraphNode(this, opcode + SSAGraphNode::SUB);
    SSAGraphNode *node1, *node2;
    if (se1->isConstant()) {
        int val1 = ((ConstantSymbolEntry*)se1)->getValue();
        node1 = new SSAGraphNode(val1);
    } else
        node1 = operands[1]->getDef()->getNode();
    if (se2->isConstant()) {
        int val2 = ((ConstantSymbolEntry*)se2)->getValue();
        node2 = new SSAGraphNode(val2);
    } else
        node2 = operands[2]->getDef()->getNode();
    node->addChild(node1);
    node->addChild(node2);
    return true;
}

bool CmpInstruction::genNode() {
    node = new SSAGraphNode(this, opcode + SSAGraphNode::E);
    auto se1 = operands[1]->getEntry();
    auto se2 = operands[2]->getEntry();
    SSAGraphNode *node1, *node2;
    if (se1->isConstant()) {
        int val1 = ((ConstantSymbolEntry*)se1)->getValue();
        node1 = new SSAGraphNode(val1);
    } else
        node1 = operands[1]->getDef()->getNode();
    if (se2->isConstant()) {
        int val2 = ((ConstantSymbolEntry*)se2)->getValue();
        node2 = new SSAGraphNode(val2);
    } else
        node2 = operands[2]->getDef()->getNode();
    node->addChild(node1);
    node->addChild(node2);
    return true;
}

bool CallInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::CALL);
    return true;
}

bool ZextInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::ZEXT);
    auto node1 = operands[1]->getDef()->getNode();
    node->addChild(node1);
    return true;
}

bool XorInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::XOR);
    auto se1 = operands[1]->getEntry();
    SSAGraphNode* node1;
    if (se1->isConstant()) {
        int val1 = ((ConstantSymbolEntry*)se1)->getValue();
        node1 = new SSAGraphNode(val1);
    } else
        node1 = operands[1]->getDef()->getNode();
    node->addChild(node1);
    return true;
}

bool BitcastInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::BITCAST);
    auto node1 = operands[1]->getDef()->getNode();
    node->addChild(node1);
    return true;
}

bool GepInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::GEP);
    auto node1 = operands[1]->getDef()->getNode();
    node->addChild(node1);
    SSAGraphNode* node2;
    auto se2 = operands[2]->getEntry();
    if (se2->isConstant()) {
        int val2 = ((ConstantSymbolEntry*)se2)->getValue();
        node2 = new SSAGraphNode(val2);
    } else
        node2 = operands[2]->getDef()->getNode();
    node->addChild(node2);
    return true;
}

bool PhiInstruction::genNode() {
    bool ret = true;
    node = new SSAGraphNode(this, SSAGraphNode::PHI);
    for (int i = 1; i < (int)operands.size(); i++) {
        auto operand = operands[i];
        auto se = operand->getEntry();
        SSAGraphNode* node1;
        if (se->isConstant()) {
            int val = ((ConstantSymbolEntry*)se)->getValue();
            node1 = new SSAGraphNode(val);
            node->addChild(node1);
        } else {
            node1 = operand->getDef()->getNode();
            if (!node1)
                ret = false;
        }
        node->addChild(node1);
    }
    return ret;
}

bool PhiInstruction::reGenNode() {
    bool ret = true;
    auto& children = node->getChildren();
    for (int i = 0; i < (int)children.size(); i++) {
        auto& child = children[i];
        if (!child) {
            auto operand = operands[i + 1];
            auto se = operand->getEntry();
            assert(se->isTemporary());
            child = operand->getDef()->getNode();
            if (!child)
                ret = false;
            children[i] = child;
        }
    }
    return ret;
}

std::string LoadInstruction::getHash() {
    // if (operands[1]->getEntry()->isVariable())
    //     return "";
    std::stringstream s;
    s << "load ";
    s << operands[1]->toStr();
    return s.str();
}

std::string BinaryInstruction::getHash() {
    std::stringstream s;
    switch (opcode) {
        case SUB:
            s << "sub ";
            break;
        case ADD:
            s << "add ";
            break;
        case AND:
            s << "and ";
            break;
        case OR:
            s << "or ";
            break;
        case MUL:
            s << "mul ";
            break;
        case DIV:
            s << "div ";
            break;
        case MOD:
            s << "mod ";
            break;
    }
    auto ope1 = operands[1];
    auto ope2 = operands[2];
    // 可交换的考虑顺序
    // const 在前 temp在后
    // 小的在前 大的在后
    if (opcode >= ADD && opcode <= MUL) {
        auto se1 = ope1->getEntry();
        auto se2 = ope2->getEntry();
        if (se2->isConstant()) {
            if (se1->isConstant()) {
                auto str1 = se1->toStr();
                auto str2 = se2->toStr();
                if (str1 < str2)
                    s << str1 << " " << str2;
                else
                    s << str2 << " " << str1;
            } else {
                s << ope2->toStr() << " " << ope1->toStr();
            }
        } else {
            if (se1->isConstant()) {
                s << ope1->toStr() << " " << ope2->toStr();
            } else {
                auto str1 = se1->toStr();
                auto str2 = se2->toStr();
                if (str1 < str2)
                    s << str1 << " " << str2;
                else
                    s << str2 << " " << str1;
            }
        }
    } else {
        s << ope1->toStr() << " " << ope2->toStr();
    }
    return s.str();
}

std::string CmpInstruction::getHash() {
    std::stringstream s;
    switch (opcode) {
        case E:
            s << "e ";
            break;
        case NE:
            s << "ne ";
            break;
        case L:
            s << "l ";
            break;
        case LE:
            s << "le ";
            break;
        case G:
            s << "g ";
            break;
        case GE:
            s << "ge ";
            break;
    }
    s << operands[1]->toStr() << " " << operands[2]->toStr();
    return s.str();
}

std::string XorInstruction::getHash() {
    std::stringstream s;
    s << "xor ";
    s << operands[1]->toStr();
    return s.str();
}

std::string GepInstruction::getHash() {
    std::stringstream s;
    s << "gep ";
    s << operands[1]->toStr() << " " << operands[2]->toStr();
    return s.str();
}

std::string PhiInstruction::getHash() {
    std::stringstream s;
    s << "phi";
    std::vector<std::string> strs;
    // for (auto it = operands.begin() + 1; it != operands.end(); it++) {
    //     strs.push_back((*it)->toStr());
    // }
    for (auto it : srcs) {
        std::stringstream ss;
        auto block = it.first;
        ss << "B" << block->getNo() << " " << it.second->toStr();
        strs.push_back(ss.str());
    }
    std::sort(strs.begin(), strs.end());
    for (auto str : strs)
        s << " " << str;
    return s.str();
}

ShlInstruction::ShlInstruction(Operand* dst,
                               Operand* src,
                               Operand* num,
                               BasicBlock* insert_bb)
    : Instruction(SHL, insert_bb), dst(dst), src(src), num(num) {
    operands.push_back(dst);
    operands.push_back(src);
    operands.push_back(num);
    dst->setDef(this);
    src->addUse(this);
    num->addUse(this);
}

void ShlInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    } else if (operands[2] == old) {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void ShlInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

ShlInstruction::~ShlInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void ShlInstruction::output() const {
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string num = operands[2]->toStr();
    std::string src_type;
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = shl %s %s, %s\n", dst.c_str(), src_type.c_str(),
            src.c_str(), num.c_str());
}

bool ShlInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::SHL);
    auto se1 = operands[1]->getEntry();
    auto se2 = operands[2]->getEntry();
    SSAGraphNode *node1, *node2;
    if (se1->isConstant()) {
        int val1 = ((ConstantSymbolEntry*)se1)->getValue();
        node1 = new SSAGraphNode(val1);
    } else
        node1 = operands[1]->getDef()->getNode();
    if (se2->isConstant()) {
        int val2 = ((ConstantSymbolEntry*)se2)->getValue();
        node2 = new SSAGraphNode(val2);
    } else
        node2 = operands[2]->getDef()->getNode();
    node->addChild(node1);
    node->addChild(node2);
    return true;
}

std::string ShlInstruction::getHash() {
    std::stringstream s;
    s << "shl ";
    s << operands[1]->toStr() << " " << operands[2]->toStr();
    return s.str();
}

std::pair<int, int> ShlInstruction::getLatticeValue(std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res, val1, val2;
    if (value.find(src) == value.end())
        val1 = src->getInitLatticeValue();
    else
        val1 = value[src];
    if (value.find(num) == value.end())
        val2 = num->getInitLatticeValue();
    else
        val2 = value[num];

    if (val1.first == -1 || val2.first == -1)
        res = {-1, 0};
    else if (val1.first == 0 && val2.first == 0)
    {
        res.first = 0;
        res.second = val1.second << val2.second;
    }
    else
        res.first = 1;
    return res;
}

void ShlInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto num = genMachineOperand(operands[2]);
    // 目前只是立即数
    assert(num->isImm());
    if (src->isImm()) {
        auto temp = genMachineVReg();
        cur_block->InsertInst(
            new LoadMInstruction(cur_block, LoadMInstruction::LDR, temp, src));
        src = new MachineOperand(*temp);
    }
    auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOVLSL, dst,
                                        src, MachineInstruction::NONE, num);
    cur_block->InsertInst(cur_inst);
}

AshrInstruction::AshrInstruction(Operand* dst,
                                 Operand* src,
                                 Operand* num,
                                 BasicBlock* insert_bb)
    : Instruction(ASHR, insert_bb), dst(dst), src(src), num(num) {
    operands.push_back(dst);
    operands.push_back(src);
    operands.push_back(num);
    dst->setDef(this);
    src->addUse(this);
    num->addUse(this);
}

void AshrInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    } else if (operands[2] == old) {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void AshrInstruction::replaceDef(Operand* new_) {
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

AshrInstruction::~AshrInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

std::pair<int, int> AshrInstruction::getLatticeValue(std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res, val1, val2;
    if (value.find(src) == value.end())
        val1 = src->getInitLatticeValue();
    else
        val1 = value[src];
    if (value.find(num) == value.end())
        val2 = num->getInitLatticeValue();
    else
        val2 = value[num];

    if (val1.first == -1 || val2.first == -1)
        res = {-1, 0};
    else if (val1.first == 0 && val2.first == 0)
    {
        res.first = 0;
        res.second = val1.second >> val2.second;
    }
    else
        res.first = 1;
    return res;
}

void AshrInstruction::output() const {
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string num = operands[2]->toStr();
    std::string src_type;
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = ashr %s %s, %s\n", dst.c_str(), src_type.c_str(),
            src.c_str(), num.c_str());
}

bool AshrInstruction::genNode() {
    node = new SSAGraphNode(this, SSAGraphNode::ASHR);
    auto se1 = operands[1]->getEntry();
    auto se2 = operands[2]->getEntry();
    SSAGraphNode *node1, *node2;
    if (se1->isConstant()) {
        int val1 = ((ConstantSymbolEntry*)se1)->getValue();
        node1 = new SSAGraphNode(val1);
    } else
        node1 = operands[1]->getDef()->getNode();
    if (se2->isConstant()) {
        int val2 = ((ConstantSymbolEntry*)se2)->getValue();
        node2 = new SSAGraphNode(val2);
    } else
        node2 = operands[2]->getDef()->getNode();
    node->addChild(node1);
    node->addChild(node2);
    return true;
}

std::string AshrInstruction::getHash() {
    std::stringstream s;
    s << "shl ";
    s << operands[1]->toStr() << " " << operands[2]->toStr();
    return s.str();
}

void AshrInstruction::genMachineCode(AsmBuilder* builder) {
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto num = genMachineOperand(operands[2]);
    // 目前只是立即数
    assert(num->isImm());
    if (src->isImm()) {
        auto temp = genMachineVReg();
        cur_block->InsertInst(
            new LoadMInstruction(cur_block, LoadMInstruction::LDR, temp, src));
        src = new MachineOperand(*temp);
    }
    auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOVASR, dst,
                                        src, MachineInstruction::NONE, num);
    cur_block->InsertInst(cur_inst);
}

bool BinaryInstruction::isConstExp() {
    bool flag = false;
    auto src1 = operands[1];
    auto src2 = operands[2];
    if (src1->isConst() && src2->isConst()) {
        flag = true;
        double val1 = src1->getConstVal();
        double val2 = src2->getConstVal();
        switch (opcode) {
            case SUB:
                constVal = val1 - val2;
                break;
            case ADD:
                constVal = val1 + val2;
                break;
            case AND:
                constVal = val1 && val2;
                break;
            case OR:
                constVal = val1 || val2;
                break;
            case MUL:
                constVal = val1 * val2;
                break;
            case DIV:
                constVal = val1 / val2;
                break;
            case MOD:
                constVal = (int)val1 % (int)val2;
                break;
        }
    }
    return flag;
}

bool CmpInstruction::isConstExp() {
    bool flag = false;
    auto src1 = operands[1];
    auto src2 = operands[2];
    if (src1->isConst() && src2->isConst()) {
        flag = true;
        double val1 = src1->getConstVal();
        double val2 = src2->getConstVal();
        switch (opcode) {
            case E:
                constVal = val1 == val2;
                break;
            case NE:
                constVal = val1 != val2;
                break;
            case L:
                constVal = val1 < val2;
                break;
            case LE:
                constVal = val1 <= val2;
                break;
            case G:
                constVal = val1 > val2;
                break;
            case GE:
                constVal = val1 >= val2;
                break;
        }
    }
    return flag;
}

bool XorInstruction::isConstExp() {
    bool flag = false;
    auto src = operands[1];
    if (src->isConst()) {
        flag = true;
        double val = src->getConstVal();
        assert(val == 0 || val == 1);
        if (val)
            constVal = 0;
        else
            constVal = 1;
    }
    return flag;
}

bool ShlInstruction::isConstExp() {
    bool flag = false;
    auto src1 = operands[1];
    auto src2 = operands[2];
    if (src1->isConst() && src2->isConst()) {
        flag = true;
        int val1 = src1->getConstVal();
        int val2 = src2->getConstVal();
        constVal = val1 << val2;
    }
    return flag;
}

bool AshrInstruction::isConstExp() {
    bool flag = false;
    auto src1 = operands[1];
    auto src2 = operands[2];
    if (src1->isConst() && src2->isConst()) {
        flag = true;
        int val1 = src1->getConstVal();
        int val2 = src2->getConstVal();
        constVal = val1 >> val2;
    }
    return flag;
}

Instruction* AllocaInstruction::copy() {
    return new AllocaInstruction(*this);
}

Instruction* LoadInstruction::copy() {
    return new LoadInstruction(*this);
}

Instruction* StoreInstruction::copy() {
    return new StoreInstruction(*this);
}

Instruction* BinaryInstruction::copy() {
    return new BinaryInstruction(*this);
}

Instruction* CmpInstruction::copy() {
    return new CmpInstruction(*this);
}

Instruction* UncondBrInstruction::copy() {
    return new UncondBrInstruction(*this);
}

Instruction* CondBrInstruction::copy() {
    auto ret = new CondBrInstruction(*this);
    ret->cleanOriginFalse();
    ret->cleanOriginTrue();
    return ret;
}

Instruction* RetInstruction::copy() {
    return new RetInstruction(*this);
}

Instruction* CallInstruction::copy() {
    return new CallInstruction(*this);
}

Instruction* ZextInstruction::copy() {
    return new ZextInstruction(*this);
}

Instruction* XorInstruction::copy() {
    return new XorInstruction(*this);
}

Instruction* GepInstruction::copy() {
    return new GepInstruction(*this);
}

Instruction* PhiInstruction::copy() {
    return new PhiInstruction(*this);
}

Instruction* FptosiInstruction::copy() {
    return new FptosiInstruction(*this);
}

Instruction* SitofpInstruction::copy() {
    return new SitofpInstruction(*this);
}

Instruction* BitcastInstruction::copy() {
    return new BitcastInstruction(*this);
}

Instruction* ShlInstruction::copy() {
    return new ShlInstruction(*this);
}

Instruction* AshrInstruction::copy() {
    return new AshrInstruction(*this);
}

void BitcastInstruction::replaceUse(Operand* old, Operand* new_) {
    if (operands[1] == old) {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}
