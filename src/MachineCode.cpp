#include "MachineCode.h"
#include <iostream>
#include <sstream>
#include "Type.h"
extern FILE* yyout;

int MachineBlock::label = 0;

MachineOperand::MachineOperand(int tp, int val, bool fpu) {
    this->type = tp;
    this->fpu = fpu;
    if (tp == MachineOperand::IMM) {
        this->val = val;
    } else {
        this->reg_no = val;  // reg_no can be float if larger than 15(pc)
    }
}

MachineOperand::MachineOperand(std::string label) {
    this->type = MachineOperand::LABEL;
    this->label = label;
}

MachineOperand::MachineOperand(int tp, float fval) {
    this->type = tp;
    if (tp == MachineOperand::IMM) {
        this->fval = fval;
        this->fpu = true;
    } else {
        // error
    }
}

uint32_t MachineOperand::getBinVal() {
    uint32_t bin_val;
    if (fpu) {
        bin_val = reinterpret_cast<uint32_t&>(fval);
    } else {
        bin_val = reinterpret_cast<uint32_t&>(val);
    }
    return bin_val;
}

bool MachineOperand::operator==(const MachineOperand& a) const {
    if (this->type != a.type)
        return false;
    if (this->fpu != a.fpu) {
        return false;
    }
    if (this->type == IMM) {
        if (this->fpu) {
            return this->fval == a.fval;
        } else {
            return this->val == a.val;
        }
    } else if (this->type == LABEL)
        return this->label == a.label;
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand& a) const {
    if (this->type == a.type) {
        if (this->type == IMM) {
            if (this->fpu && a.fpu) {
                return this->fval < a.fval;
            } else if (!this->fpu && !a.fpu) {
                return this->val < a.val;
            } else if (this->fpu && !a.fpu) {
                return this->fval < a.val;
            } else if (!this->fpu && a.fpu) {
                return this->val < a.fval;
            }
        } else if (this->type == LABEL) {
            return this->label < a.label;
        }
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

void MachineOperand::PrintReg() {
    if (reg_no >= 16) {
        int sreg_no = reg_no - 16;
        if (sreg_no <= 31) {
            fprintf(yyout, "s%d", sreg_no);
        } else if (sreg_no == 32) {
            fprintf(yyout, "FPSCR");
        }
    } else if (reg_no == 11) {
        fprintf(yyout, "fp");

    } else if (reg_no == 13) {
        fprintf(yyout, "sp");

    } else if (reg_no == 14) {
        fprintf(yyout, "lr");

    } else if (reg_no == 15) {
        fprintf(yyout, "pc");

    } else {
        fprintf(yyout, "r%d", reg_no);
    }
}

void MachineOperand::output() {
    /* HINT：print operand
     * Example:
     * immediate num 1 -> print #1;
     * register 1 -> print r1;
     * lable addr_a -> print addr_a; */
    switch (this->type) {
        case IMM:
            if (!fpu) {
                fprintf(yyout, "#%d", this->val);
            } else {
                uint32_t temp = reinterpret_cast<uint32_t&>(this->fval);
                fprintf(yyout, "#%u", temp);
            }
            break;
        case VREG:
            fprintf(yyout, "v%d", this->reg_no);
            break;
        case REG:
            PrintReg();
            break;
        case LABEL:
            if (this->label.substr(0, 2) == ".L")
                fprintf(yyout, "%s", this->label.c_str());
            else if (this->label.substr(0, 1) == "@")
                fprintf(yyout, "%s", this->label.c_str() + 1);
            else
                fprintf(yyout, "addr_%s%d", this->label.c_str(),
                        parent->getParent()->getParent()->getParent()->getN());
        default:
            break;
    }
}

void MachineInstruction::PrintCond() {
    switch (cond) {
        case EQ:
            fprintf(yyout, "eq");
            break;
        case NE:
            fprintf(yyout, "ne");
            break;
        case LT:
            fprintf(yyout, "lt");
            break;
        case LE:
            fprintf(yyout, "le");
            break;
        case GT:
            fprintf(yyout, "gt");
            break;
        case GE:
            fprintf(yyout, "ge");
            break;
        default:
            break;
    }
}

void MachineInstruction::insertBefore(MachineInstruction* inst) {
    auto& instructions = parent->getInsts();
    auto it = std::find(instructions.begin(), instructions.end(), this);
    instructions.insert(it, inst);
}

void MachineInstruction::insertAfter(MachineInstruction* inst) {
    auto& instructions = parent->getInsts();
    auto it = std::find(instructions.begin(), instructions.end(), this);
    instructions.insert(++it, inst);
}

void MachineInstruction::replaceDef(MachineOperand* old, MachineOperand* new_) {
    for (auto i = 0; i < (int)def_list.size(); i++)
        if (def_list[i] == old) {
            def_list[i] = new_;
            new_->setParent(this);
            break;
        }
}

void MachineInstruction::replaceUse(MachineOperand* old, MachineOperand* new_) {
    for (auto i = 0; i < (int)use_list.size(); i++)
        if (use_list[i] == old) {
            use_list[i] = new_;
            new_->setParent(this);
            break;
        }
}

FuseMInstruction::FuseMInstruction(MachineBlock* p,
                                   int op,
                                   MachineOperand* dst,
                                   MachineOperand* src1,
                                   MachineOperand* src2,
                                   MachineOperand* src3) {
    this->parent = p;
    this->type = MachineInstruction::FUSE;
    this->op = op;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    this->use_list.push_back(src3);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
    src3->setParent(this);
}
void FuseMInstruction::output() {
    switch (this->op) {
        case FuseMInstruction::MLA:
            fprintf(yyout, "\tmla ");
            break;
        case FuseMInstruction::MLS:
            fprintf(yyout, "\tmls ");
            break;
        default:
            break;
    }

    this->PrintCond();
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, ", ");
    this->use_list[2]->output();
    fprintf(yyout, "\n");
}

BinaryMInstruction::BinaryMInstruction(MachineBlock* p,
                                       int op,
                                       MachineOperand* dst,
                                       MachineOperand* src1,
                                       MachineOperand* src2,
                                       int cond) {
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output() {
    // 这里面PrintCond应该是用不到的啊
    switch (this->op) {
        case BinaryMInstruction::ADD:
            fprintf(yyout, "\tadd ");
            break;
        case BinaryMInstruction::SUB:
            fprintf(yyout, "\tsub ");
            break;
        case BinaryMInstruction::AND:
            fprintf(yyout, "\tand ");
            break;
        case BinaryMInstruction::OR:
            fprintf(yyout, "\torr ");
            break;
        case BinaryMInstruction::MUL:
            fprintf(yyout, "\tmul ");
            break;
        case BinaryMInstruction::DIV:
            fprintf(yyout, "\tsdiv ");
            break;
        case BinaryMInstruction::VADD:
            fprintf(yyout, "\tvadd.f32 ");
            break;
        case BinaryMInstruction::VSUB:
            fprintf(yyout, "\tvsub.f32 ");
            break;
        case BinaryMInstruction::VMUL:
            fprintf(yyout, "\tvmul.f32 ");
            break;
        case BinaryMInstruction::VDIV:
            fprintf(yyout, "\tvdiv.f32 ");
            break;
        default:
            break;
    }

    this->PrintCond();
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

LoadMInstruction::LoadMInstruction(MachineBlock* p,
                                   int op,
                                   MachineOperand* dst,
                                   MachineOperand* src1,
                                   MachineOperand* src2,
                                   int cond) {
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = op;
    this->cond = cond;
    this->needModify = false;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}

void LoadMInstruction::output() {
    if (op == LoadMInstruction::LDR) {
        fprintf(yyout, "\tldr ");
        this->def_list[0]->output();
        fprintf(yyout, ", ");

        // Load immediate num, eg: ldr r1, =8
        if (this->use_list[0]->isImm()) {
            if (this->use_list[0]->isFloat()) {
                float fval = this->use_list[0]->getFVal();
                uint32_t temp = reinterpret_cast<uint32_t&>(fval);
                fprintf(yyout, "=%u\n", temp);
            } else {
                fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
            }
            return;
        }

        // Load address
        if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
            fprintf(yyout, "[");

        this->use_list[0]->output();
        if (this->use_list.size() > 1) {
            fprintf(yyout, ", ");
            this->use_list[1]->output();
        }

        if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
            fprintf(yyout, "]");
        fprintf(yyout, "\n");

    } else if (op == LoadMInstruction::VLDR) {
        fprintf(yyout, "\tvldr.32 ");
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        // Load immediate num, eg: ldr r1, =8
        if (this->use_list[0]->isImm()) {
            if (this->use_list[0]->isFloat()) {
                float fval = this->use_list[0]->getFVal();
                uint32_t temp = reinterpret_cast<uint32_t&>(fval);
                fprintf(yyout, "=%u\n", temp);
            } else {
                fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
            }
            return;
        }

        // Load address
        if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
            fprintf(yyout, "[");

        this->use_list[0]->output();
        if (this->use_list.size() > 1) {
            fprintf(yyout, ", ");
            this->use_list[1]->output();
        }

        if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
            fprintf(yyout, "]");
        fprintf(yyout, "\n");
    }
}

StoreMInstruction::StoreMInstruction(MachineBlock* p,
                                     int op,
                                     MachineOperand* src1,
                                     MachineOperand* src2,
                                     MachineOperand* src3,
                                     int cond) {
    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = op;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    if (src3)
        this->use_list.push_back(src3);
    src1->setParent(this);
    src2->setParent(this);
    if (src3)
        src3->setParent(this);
}

void StoreMInstruction::output() {
    if (op == StoreMInstruction::STR) {
        fprintf(yyout, "\tstr ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        // store address
        if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
            fprintf(yyout, "[");
        this->use_list[1]->output();
        if (this->use_list.size() > 2) {
            fprintf(yyout, ", ");
            this->use_list[2]->output();
        }
        if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
            fprintf(yyout, "]");
        fprintf(yyout, "\n");
    } else if (op == StoreMInstruction::VSTR) {
        // TODO
        fprintf(yyout, "\tvstr.32 ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        // store address
        if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
            fprintf(yyout, "[");
        this->use_list[1]->output();
        if (this->use_list.size() > 2) {
            fprintf(yyout, ", ");
            this->use_list[2]->output();
        }
        if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
            fprintf(yyout, "]");
        fprintf(yyout, "\n");
    }
}

MovMInstruction::MovMInstruction(MachineBlock* p,
                                 int op,
                                 MachineOperand* dst,
                                 MachineOperand* src,
                                 int cond,
                                 MachineOperand* num) {
    this->parent = p;
    this->type = MachineInstruction::MOV;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
    if (num) {
        assert(op == MOVASR || op == MOVLSL);
        this->use_list.push_back(num);
        num->setParent(this);
    }
}

void MovMInstruction::output() {
    switch (this->op) {
        case MovMInstruction::MOV:
        case MovMInstruction::MOVLSL:
        case MovMInstruction::MOVASR:
            fprintf(yyout, "\tmov");
            break;
        case MovMInstruction::MOVT:
            fprintf(yyout, "\tmovt");
            break;
        case MovMInstruction::VMOV:
            fprintf(yyout, "\tvmov");
            break;
        case MovMInstruction::VMOVF32:
            fprintf(yyout, "\tvmov.f32");
            break;
        default:
            break;
    }
    PrintCond();
    fprintf(yyout, " ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    if (op == MOVLSL) {
        fprintf(yyout, ", LSL");
        this->use_list[1]->output();
    }
    if (op == MOVASR) {
        fprintf(yyout, ", ASR");
        this->use_list[1]->output();
    }
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock* p,
                                       int op,
                                       MachineOperand* dst,
                                       int cond) {
    this->parent = p;
    this->type = MachineInstruction::BRANCH;
    this->op = op;
    this->cond = cond;
    // label有必要存吗
    // if (op != BL) {
    this->use_list.push_back(dst);
    dst->setParent(this);
    // }else{
    if (op == BL) {
        auto r0d = new MachineOperand(MachineOperand::REG, 0);
        auto r0u = new MachineOperand(MachineOperand::REG, 0);
        auto r1d = new MachineOperand(MachineOperand::REG, 1);
        auto r1u = new MachineOperand(MachineOperand::REG, 1);
        auto r2d = new MachineOperand(MachineOperand::REG, 2);
        auto r2u = new MachineOperand(MachineOperand::REG, 2);
        auto r3d = new MachineOperand(MachineOperand::REG, 3);
        auto r3u = new MachineOperand(MachineOperand::REG, 3);
        r0d->setParent(this);
        r0u->setParent(this);
        r1d->setParent(this);
        r1u->setParent(this);
        r2d->setParent(this);
        r2u->setParent(this);
        r3d->setParent(this);
        r3u->setParent(this);
        this->def_list.push_back(r0d);
        this->def_list.push_back(r1d);
        this->def_list.push_back(r2d);
        this->def_list.push_back(r3d);
        this->use_list.push_back(r0u);
        this->use_list.push_back(r1u);
        this->use_list.push_back(r2u);
        this->use_list.push_back(r3u);
    } else if (op == BX) {
        auto r0 = new MachineOperand(MachineOperand::REG, 0);
        auto sp = new MachineOperand(MachineOperand::REG, 13);
        r0->setParent(this);
        sp->setParent(this);
        this->use_list.push_back(r0);
        this->use_list.push_back(sp);
    }
}

void BranchMInstruction::output() {
    switch (op) {
        case B:
            fprintf(yyout, "\tb");
            break;
        case BX:
            fprintf(yyout, "\tbx");
            break;
        case BL:
            fprintf(yyout, "\tbl");
            break;
    }
    PrintCond();
    fprintf(yyout, " ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

CmpMInstruction::CmpMInstruction(MachineBlock* p,
                                 int op,
                                 MachineOperand* src1,
                                 MachineOperand* src2,
                                 int cond) {
    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->op = op;
    this->cond = cond;
    p->setCmpCond(cond);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
}

void CmpMInstruction::output() {
    switch (this->op) {
        case CmpMInstruction::CMP:
            fprintf(yyout, "\tcmp ");
            break;
        case CmpMInstruction::VCMP:
            fprintf(yyout, "\tvcmp.f32 ");
            break;
        default:
            break;
    }
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

StackMInstruction::StackMInstruction(MachineBlock* p,
                                     int op,
                                     std::vector<MachineOperand*> srcs,
                                     MachineOperand* src,
                                     MachineOperand* src1,
                                     int cond) {
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    if (srcs.size()) {
        for (auto it = srcs.begin(); it != srcs.end(); it++) {
            this->use_list.push_back(*it);
        }
    }
    if (src) {
        this->use_list.push_back(src);
        src->setParent(this);
    }
    if (src1) {
        this->use_list.push_back(src1);
        src1->setParent(this);
    }
}

VcvtMInstruction::VcvtMInstruction(MachineBlock* p,
                                   int op,
                                   MachineOperand* dst,
                                   MachineOperand* src,
                                   int cond) {
    this->parent = p;
    this->type = MachineInstruction::VCVT;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

VmrsMInstruction::VmrsMInstruction(MachineBlock* p) {
    this->parent = p;
    this->type = MachineInstruction::VMRS;
}

void VmrsMInstruction::output() {
    fprintf(yyout, "\tvmrs APSR_nzcv, FPSCR\n");
}

void VcvtMInstruction::output() {
    switch (this->op) {
        case VcvtMInstruction::F2S:
            fprintf(yyout, "\tvcvt.s32.f32 ");
            break;
        case VcvtMInstruction::S2F:
            fprintf(yyout, "\tvcvt.f32.s32 ");
            break;
        default:
            break;
    }
    PrintCond();
    fprintf(yyout, " ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

void StackMInstruction::output() {
    if (!this->use_list.empty()) {
        switch (op) {
            case PUSH:
                fprintf(yyout, "\tpush ");
                break;
            case POP:
                fprintf(yyout, "\tpop ");
                break;
            case VPUSH:
                fprintf(yyout, "\tvpush ");
                break;
            case VPOP:
                fprintf(yyout, "\tvpop ");
                break;
        }
        fprintf(yyout, "{");
        this->use_list[0]->output();
        for (long unsigned int i = 1; i < use_list.size(); i++) {
            fprintf(yyout, ", ");
            this->use_list[i]->output();
        }
        fprintf(yyout, "}\n");
    }
}

MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) {
    this->parent = p;
    this->sym_ptr = sym_ptr;
    this->stack_size = 0;
    this->paramsNum =
        ((FunctionType*)(sym_ptr->getType()))->getParamsSe().size();
};

std::string MachineBlock::getLabel() {
    std::stringstream s;
    s << ".L" << no;
    return s.str();
}

void MachineBlock::cleanSucc() {
    for (auto s : succ)
        s->removePred(this);
    std::vector<MachineBlock*>().swap(succ);
}

void MachineBlock::removePred(MachineBlock* block) {
    pred.erase(std::find(pred.begin(), pred.end(), block));
}

void MachineBlock::removeSucc(MachineBlock* block) {
    succ.erase(std::find(succ.begin(), succ.end(), block));
}

void MachineBlock::output() {
    bool first = true;
    int offset =
        (parent->getSavedRegs().size() + parent->getSavedFpRegs().size() + 2) *
        4;
    int baseOffset = offset;
    int num = parent->getParamsNum();
    int count = 0;
    // if (!inst_list.empty()) {
    fprintf(yyout, ".L%d:\n", this->no);
    for (auto it = inst_list.begin(); it != inst_list.end(); it++) {
        if ((*it)->isBX()) {
            auto fp = new MachineOperand(MachineOperand::REG, 11);
            auto lr = new MachineOperand(MachineOperand::REG, 14);
            auto cur_inst = new StackMInstruction(this, StackMInstruction::VPOP,
                                                  parent->getSavedFpRegs());
            cur_inst->output();
            cur_inst = new StackMInstruction(this, StackMInstruction::POP,
                                             parent->getSavedRegs(), fp, lr);
            cur_inst->output();
        }
        if (num > 4 && (*it)->isStore()) {
            MachineOperand* operand = (*it)->getUse()[0];
            if (operand->isReg() && operand->getReg() == 3 &&
                operand->isParam()) {
                auto fp = new MachineOperand(MachineOperand::REG, 11);
                auto r3 = new MachineOperand(MachineOperand::REG, 3);
                int temp = baseOffset + operand->getOffset();
                auto off = new MachineOperand(MachineOperand::IMM, temp);
                // auto off = new MachineOperand(MachineOperand::IMM, offset);
                // offset += 4;
                auto cur_inst = new LoadMInstruction(
                    this, LoadMInstruction::LDR, r3, fp, off);
                cur_inst->output();
            } else if (operand->isReg() && operand->getReg() == 19) {
                // floating point
                if (first) {
                    first = false;
                } else {
                    auto fp = new MachineOperand(MachineOperand::REG, 11);
                    auto s3 = new MachineOperand(MachineOperand::REG, 19, true);
                    auto off = new MachineOperand(MachineOperand::IMM, offset);
                    offset += 4;
                    auto cur_inst = new LoadMInstruction(
                        this, LoadMInstruction::VLDR, s3, fp, off);
                    cur_inst->output();
                }
            }
        }
        if (num > 4 && (*it)->isAdd()) {
            auto uses = (*it)->getUse();
            if (uses[0]->isParam() && uses[1]->isImm() &&
                uses[1]->getVal() == 0) {
                auto fp = new MachineOperand(MachineOperand::REG, 11);
                auto r3 = new MachineOperand(MachineOperand::REG, 3);
                int temp = baseOffset + uses[0]->getOffset();
                auto off = new MachineOperand(MachineOperand::IMM, temp);
                // auto off = new MachineOperand(MachineOperand::IMM, offset);
                // offset += 4;
                auto cur_inst = new LoadMInstruction(
                    this, LoadMInstruction::LDR, r3, fp, off);
                cur_inst->output();
            }
        }
        if ((*it)->isLoad() && ((LoadMInstruction*)(*it))->isNeedModify()) {
            auto imm = (*it)->getUse()[1];
            imm->setVal(imm->getVal() + baseOffset);
        }
        if ((*it)->isAdd()) {
            auto dst = (*it)->getDef()[0];
            auto src1 = (*it)->getUse()[0];
            if (dst->isReg() && dst->getReg() == 13 && src1->isReg() &&
                src1->getReg() == 13 && (*(it + 1))->isBX()) {
                int size = parent->AllocSpace(0);
                if (size < -255 || size > 255) {
                    auto r1 = new MachineOperand(MachineOperand::REG, 1);
                    auto off = new MachineOperand(MachineOperand::IMM, size);
                    (new LoadMInstruction(nullptr, LoadMInstruction::LDR, r1,
                                          off))
                        ->output();
                    (*it)->getUse()[1]->setReg(1);
                } else
                    (*it)->getUse()[1]->setVal(size);
            }
        }
        (*it)->output();
        count++;
        if (count % 500 == 0) {
            fprintf(yyout, "\tb .B%d\n", label);
            fprintf(yyout, ".LTORG\n");
            parent->getParent()->printGlobal();
            fprintf(yyout, ".B%d:\n", label++);
        }
    }
    // }
}

void MachineFunction::output() {
    fprintf(yyout, "\t.global %s\n", this->sym_ptr->toStr().c_str() + 1);
    fprintf(yyout, "\t.type %s , %%function\n",
            this->sym_ptr->toStr().c_str() + 1);
    fprintf(yyout, "%s:\n", this->sym_ptr->toStr().c_str() + 1);
    /* Hint:
     *  1. Save fp
     *  2. fp = sp
     *  3. Save callee saved register
     *  4. Allocate stack space for local variable */

    // Traverse all the block in block_list to print assembly code.
    auto fp = new MachineOperand(MachineOperand::REG, 11);
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto lr = new MachineOperand(MachineOperand::REG, 14);
    (new StackMInstruction(nullptr, StackMInstruction::PUSH, getSavedRegs(), fp,
                           lr))
        ->output();
    (new StackMInstruction(nullptr, StackMInstruction::VPUSH, getSavedFpRegs()))
        ->output();
    (new MovMInstruction(nullptr, MovMInstruction::MOV, fp, sp))->output();
    int off = AllocSpace(0);
    if (off % 8 != 0) {
        off = AllocSpace(4);
    }
    auto size = new MachineOperand(MachineOperand::IMM, off);
    if (off < -255 || off > 255) {
        auto r4 = new MachineOperand(MachineOperand::REG, 4);
        (new LoadMInstruction(nullptr, LoadMInstruction::LDR, r4, size))
            ->output();
        (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, sp, sp, r4))
            ->output();
    } else {
        (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, sp, sp, size))
            ->output();
    }
    int count = 0;
    for (auto iter : block_list) {
        iter->output();
        count += iter->getSize();
        if (count > 160) {
            fprintf(yyout, "\tb .F%d\n", parent->getN());
            fprintf(yyout, ".LTORG\n");
            parent->printGlobal();
            fprintf(yyout, ".F%d:\n", parent->getN() - 1);
            count = 0;
        }
    }
    fprintf(yyout, "\n");
}

void MachineFunction::addSavedRegs(int regno) {
    if (regno < 16) {
        // Q&A about this alignment:
        // https://forums.raspberrypi.com/viewtopic.php?t=169192
        saved_regs.insert(regno);
        if (regno <= 11 && regno % 2 != 0) {
            saved_regs.insert(regno + 1);
        } else if (regno <= 11 && regno > 0 && regno % 2 == 0) {
            saved_regs.insert(regno - 1);
        }
    } else {
        saved_fpregs.insert(regno);
    }
};

std::vector<MachineOperand*> MachineFunction::getSavedRegs() {
    std::vector<MachineOperand*> regs;
    for (auto it = saved_regs.begin(); it != saved_regs.end(); it++) {
        auto reg = new MachineOperand(MachineOperand::REG, *it);
        regs.push_back(reg);
    }
    return regs;
}

std::vector<MachineOperand*> MachineFunction::getSavedFpRegs() {
    int min_regno = 31 + 16;
    int max_regno = 0 + 16;
    for (auto it = saved_fpregs.begin(); it != saved_fpregs.end(); it++) {
        if (*it > max_regno) {
            max_regno = *it;
        }
        if (*it < min_regno) {
            min_regno = *it;
        }
    }

    int cnt = max_regno - min_regno + 1;
    if (cnt % 2 != 0) {
        max_regno += 1;
    }

    std::vector<MachineOperand*> regs;
    for (int i = min_regno; i <= max_regno; ++i) {
        auto reg = new MachineOperand(MachineOperand::REG, i, true);
        regs.push_back(reg);
    }
    return regs;
}

void MachineUnit::PrintGlobalDecl() {
    std::vector<int> constIdx;
    std::vector<int> zeroIdx;
    if (!global_list.empty())
        fprintf(yyout, "\t.data\n");
    for (long unsigned int i = 0; i < global_list.size(); i++) {
        IdentifierSymbolEntry* se = (IdentifierSymbolEntry*)global_list[i];
        if (se->getConst()) {
            constIdx.push_back(i);
        } else if (se->isAllZero()) {
            zeroIdx.push_back(i);
        } else {
            fprintf(yyout, "\t.global %s\n", se->toStr().c_str());
            fprintf(yyout, "\t.align 4\n");
            fprintf(yyout, "\t.size %s, %lld\n", se->toStr().c_str(),
                    se->getType()->getSize() / 8);
            fprintf(yyout, "%s:\n", se->toStr().c_str());
            if (!se->getType()->isArray()) {
                if (se->getType()->isFloat()) {
                    float temp = (float)(se->getValue());
                    uint32_t val = reinterpret_cast<uint32_t&>(temp);
                    fprintf(yyout, "\t.word %u\n", val);
                } else {
                    fprintf(yyout, "\t.word %d\n", (int)se->getValue());
                }
            } else {
                int n = se->getType()->getSize() / 32;
                Type* arrTy = ((ArrayType*)(se->getType()))->getElementType();

                while (!arrTy->isFloat() && !arrTy->isInt()) {
                    arrTy = ((ArrayType*)(arrTy))->getElementType();
                }  // TODO: fix problems of arrays;

                double* p = se->getArrayValue();
                if (arrTy->isFloat()) {
                    for (int i = 0; i < n; i++) {
                        float temp = (float)p[i];
                        uint32_t val = reinterpret_cast<uint32_t&>(temp);
                        fprintf(yyout, "\t.word %u\n", val);
                    }
                } else {
                    for (int i = 0; i < n; i++) {
                        fprintf(yyout, "\t.word %d\n", (int)p[i]);
                    }
                }
            }
        }
    }
    if (!constIdx.empty()) {
        fprintf(yyout, "\t.section .rodata\n");
        for (auto i : constIdx) {
            IdentifierSymbolEntry* se = (IdentifierSymbolEntry*)global_list[i];
            fprintf(yyout, "\t.global %s\n", se->toStr().c_str());
            fprintf(yyout, "\t.align 4\n");
            fprintf(yyout, "\t.size %s, %lld\n", se->toStr().c_str(),
                    se->getType()->getSize() / 8);
            fprintf(yyout, "%s:\n", se->toStr().c_str());
            if (!se->getType()->isArray()) {
                if (se->getType()->isFloat()) {
                    float temp = (float)(se->getValue());
                    // printf("[%s]\t%f\n", se->toStr().c_str(), temp);
                    uint32_t val = reinterpret_cast<uint32_t&>(temp);
                    fprintf(yyout, "\t.word %u\n", val);
                } else {
                    fprintf(yyout, "\t.word %d\n", (int)se->getValue());
                }
            } else {
                int n = se->getType()->getSize() / 32;
                Type* arrTy = ((ArrayType*)(se->getType()))->getElementType();

                while (!arrTy->isFloat() && !arrTy->isInt()) {
                    arrTy = ((ArrayType*)(arrTy))->getElementType();
                }  // TODO: fix problems of arrays;

                double* p = se->getArrayValue();
                if (arrTy->isFloat()) {
                    for (int i = 0; i < n; i++) {
                        float temp = (float)p[i];
                        uint32_t val = reinterpret_cast<uint32_t&>(temp);
                        fprintf(yyout, "\t.word %u\n", val);
                    }
                } else {
                    for (int i = 0; i < n; i++) {
                        fprintf(yyout, "\t.word %d\n", (int)p[i]);
                    }
                }
            }
        }
    }
    if (!zeroIdx.empty()) {
        for (auto i : zeroIdx) {
            IdentifierSymbolEntry* se = (IdentifierSymbolEntry*)global_list[i];
            if (se->getType()->isArray()) {
                fprintf(yyout, "\t.comm %s, %lld, 4\n", se->toStr().c_str(),
                        se->getType()->getSize() / 8);
            }
        }
    }
}

void MachineUnit::output() {
    /* Hint:
     * 1. You need to print global variable/const declarition code;
     * 2. Traverse all the function in func_list to print assembly code;
     * 3. Don't forget print bridge label at the end of assembly code!! */
    fprintf(yyout, "\t.cpu cortex-a72\n");
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.fpu vfpv3-d16\n");
    fprintf(yyout, "\t.arch_extension crc\n");
    // fprintf(yyout, "\t.arm\n");
    PrintGlobalDecl();
    fprintf(yyout, "\t.text\n");
    int count = 0;
    for (auto iter : func_list) {
        iter->output();
        count += iter->getSize();
        if (count > 600) {
            fprintf(yyout, "\tb .F%d\n", n);
            fprintf(yyout, ".LTORG\n");
            printGlobal();
            fprintf(yyout, ".F%d:\n", n - 1);
            count = 0;
        }
    }
    // if (n == 0)
    printGlobal();
}

void MachineUnit::insertGlobal(SymbolEntry* se) {
    global_list.push_back(se);
}

void MachineUnit::printGlobal() {
    for (auto s : global_list) {
        IdentifierSymbolEntry* se = (IdentifierSymbolEntry*)s;
        fprintf(yyout, "addr_%s%d:\n", se->toStr().c_str(), n);
        fprintf(yyout, "\t.word %s\n", se->toStr().c_str());
    }
    n++;
}

bool MachineBlock::isBefore(MachineInstruction* a, MachineInstruction* b) {
    auto ait = find(inst_list.begin(), inst_list.end(), a);
    auto bit = find(inst_list.begin(), inst_list.end(), b);
    return ait < bit;
}

void MachineBlock::remove(MachineInstruction* ins) {
    auto it = find(inst_list.begin(), inst_list.end(), ins);
    if (it != inst_list.end())
        inst_list.erase(it);
}

MachineInstruction* MachineBlock::getNext(MachineInstruction* in) {
    auto it = find(inst_list.begin(), inst_list.end(), in);
    if (it != inst_list.end() && (it + 1) != inst_list.end()) {
        return *(it + 1);
    }
    return nullptr;
}

void MachineFunction::removeBlock(MachineBlock* block) {
    block_list.erase(std::find(block_list.begin(), block_list.end(), block));
}

MachineBlock* MachineFunction::getNext(MachineBlock* block) {
    auto it = find(block_list.begin(), block_list.end(), block);
    if (it != block_list.end() && (it + 1) != block_list.end()) {
        return *(it + 1);
    }
    return nullptr;
}

void MachineBlock::insertBefore(MachineInstruction* a, MachineInstruction* b) {
    auto it = find(inst_list.begin(), inst_list.end(), b);
    if (it != inst_list.end())
        inst_list.insert(it, a);
}

void MachineFunction::InsertAfter(MachineBlock* a, MachineBlock* b) {
    auto it = find(block_list.begin(), block_list.end(), a);
    if (it != block_list.end()) {
        block_list.insert(it + 1, b);
        no2Block[b->getNo()] = b;
    }
}

void MachineBlock::insertFront(MachineInstruction* in) {
    inst_list.insert(inst_list.begin(), in);
}