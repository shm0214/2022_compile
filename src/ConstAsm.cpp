
#include "ConstAsm.h"
#include <iostream>

ConstAsm::ConstAsm(MachineUnit* mUnit) {
    this->mUnit = mUnit;
}

void ConstAsm::pass() {
    for (auto func_iter = mUnit->begin(); func_iter != mUnit->end();
         func_iter++) {
        auto func = *func_iter;
        for (auto block_iter = func->begin(); block_iter != func->end();
             block_iter++) {
            op2imm.clear();
            op2inst.clear();
            auto block = *block_iter;
            simplify(block);
        }
    }
}

void ConstAsm::simplify(MachineBlock* block) {
    auto& insts = block->getInsts();
    std::set<MachineInstruction*> instToRemove;
    for (auto inst_iter = insts.begin(); inst_iter != insts.end();
         inst_iter++) {
        auto inst = *inst_iter;
        if (inst->isLoad() || inst->isMov()) {
            auto dst = inst->getDef()[0];
            auto src = inst->getUse()[0];
            if (src->isImm() && dst->isVReg()) {
                if (!dst->isFloat()) {
                    op2imm[dst->getReg()] = src->getVal();
                    op2inst[dst->getReg()] = inst;
                } else {
                    // TODO
                }
            }
        } else {
            if (inst->isBinary()) {
                auto dst = inst->getDef()[0];
                auto src1 = inst->getUse()[0];
                auto src2 = inst->getUse()[1];
                if (!src1->isVReg() || !src2->isVReg()) {
                    continue;
                }
                if (op2imm.count(src1->getReg()) &&
                    op2imm.count(src2->getReg())) {
                    if (!dst->isFloat()) {
                        int val1 = op2imm[src1->getReg()];
                        int val2 = op2imm[src2->getReg()];
                        int val3;
                        if (inst->isAdd()) {
                            val3 = val1 + val2;
                        } else if (inst->isSub()) {
                            val3 = val1 - val2;
                        } else if (inst->isMul()) {
                            val3 = val1 * val2;
                        } else if (inst->isDiv()) {
                            val3 = val1 / val2;
                        } else {
                            continue;
                        }

                        MachineInstruction* new_inst;

                        instToRemove.insert(op2inst[src1->getReg()]);
                        instToRemove.insert(op2inst[src2->getReg()]);

                        dst = new MachineOperand(*dst);

                        if (val3 < 255 && val3 > -255) {
                            new_inst = new MovMInstruction(
                                block, MovMInstruction::MOV, dst,
                                new MachineOperand(MachineOperand::IMM, val3));
                        } else {
                            new_inst = new LoadMInstruction(
                                block, LoadMInstruction::LDR, dst,
                                new MachineOperand(MachineOperand::IMM, val3));
                        }

                        *inst_iter = new_inst;

                        op2imm[dst->getReg()] = val3;
                        op2inst[dst->getReg()] = new_inst;

                    } else {
                        // TODO
                    }
                }
            }
        }
    }
    for (auto inst : instToRemove) {
        block->remove(inst);
    }
}