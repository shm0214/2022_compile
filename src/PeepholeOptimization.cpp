#include "PeepholeOptimization.h"

PeepholeOptimization::PeepholeOptimization(MachineUnit* mUnit) {
    this->mUnit = mUnit;
}

void PeepholeOptimization::pass() {
    for (auto func_iter = mUnit->begin(); func_iter != mUnit->end();
         func_iter++) {
        auto func = *func_iter;
        for (auto block_iter = func->begin(); block_iter != func->end();
             block_iter++) {
            auto block = *block_iter;
            auto curr_inst_iter = block->begin();
            auto next_inst_iter = next(curr_inst_iter, 1);

            std::set<MachineInstruction*> instToRemove;

            for (; next_inst_iter != block->end();
                 curr_inst_iter++, next_inst_iter++) {
                auto curr_inst = *curr_inst_iter;
                auto next_inst = *next_inst_iter;

                // fuse mul and add/sub
                //     mul v0, v1, v2
                //     add v3, v4, v0
                //     -----
                //     mla v3, v1, v2, v4
                //
                //     mul v0, v1, v2
                //     sub v3, v4, v0
                //     -----
                //     mls v3, v1, v2, v4

                // mla/mls rd, rn, rm, ra
                // https://developer.arm.com/documentation/dui0489/c/arm-and-thumb-instructions/multiply-instructions/mul--mla--and-mls
                // FIXME: problem at functional/71_full_conn.

                if (curr_inst->isMul() && next_inst->isAdd()) {
                    auto mul_dst = curr_inst->getDef()[0];
                    auto add_src1 = next_inst->getUse()[0];
                    auto add_src2 = next_inst->getUse()[1];

                    auto rd = new MachineOperand(*(next_inst->getDef()[0]));
                    auto rn = new MachineOperand(*(curr_inst->getUse()[0]));
                    auto rm = new MachineOperand(*(curr_inst->getUse()[1]));
                    MachineOperand* ra;

                    if (mul_dst->getReg() == add_src1->getReg()) {
                        ra = new MachineOperand(*add_src2);
                    } else if (mul_dst->getReg() == add_src2->getReg()) {
                        ra = new MachineOperand(*add_src1);
                    } else {
                        continue;
                    }

                    if (rn->isImm() || rm->isImm() || ra->isImm()) {
                        continue;
                    }

                    auto fused_inst = new FuseMInstruction(
                        block, FuseMInstruction::MLA, rd, rn, rm, ra);
                    *next_inst_iter = fused_inst;
                    instToRemove.insert(curr_inst);

                } else if (curr_inst->isMul() && next_inst->isSub()) {
                    auto mul_dst = curr_inst->getDef()[0];
                    auto sub_src = next_inst->getUse()[1];
                    if (mul_dst->getReg() != sub_src->getReg()) {
                        continue;
                    }
                    auto rd = new MachineOperand(*(next_inst->getDef()[0]));
                    auto rn = new MachineOperand(*(curr_inst->getUse()[0]));
                    auto rm = new MachineOperand(*(curr_inst->getUse()[1]));
                    auto ra = new MachineOperand(*(next_inst->getUse()[0]));

                    auto fused_inst = new FuseMInstruction(
                        block, FuseMInstruction::MLS, rd, rn, rm, ra);
                    *next_inst_iter = fused_inst;
                    instToRemove.insert(curr_inst);
                }
                // merge store and load into move
                //     str v355, [v11]
                //     ldr v227, [v11]
                //     -----
                //     str v355, [v11]
                //     mov v227, v355
                else if (curr_inst->isStore() && next_inst->isLoad()) {
                    auto src = curr_inst->getUse()[0];
                    auto str_stk_src1 = curr_inst->getUse()[1];
                    MachineOperand* str_stk_src2 = nullptr;
                    if (curr_inst->getUse().size() > 2) {
                        str_stk_src2 = curr_inst->getUse()[2];
                    }

                    auto dst = next_inst->getDef()[0];
                    auto ldr_stk_src1 = next_inst->getUse()[0];
                    MachineOperand* ldr_stk_src2 = nullptr;
                    if (next_inst->getUse().size() > 1) {
                        ldr_stk_src2 = next_inst->getUse()[1];
                    }
                    if (str_stk_src1->getReg() == ldr_stk_src1->getReg() &&
                        str_stk_src2 == nullptr && ldr_stk_src2 == nullptr) {
                        // TODO: add offset support
                        dst = new MachineOperand(*dst);
                        src = new MachineOperand(*src);
                        auto new_inst = new MovMInstruction(
                            block, MovMInstruction::MOV, dst, src);
                        // cannot determine whether the stack will be used
                        // again or not.
                        *next_inst_iter = new_inst;
                    }
                }
                // merge ldr 2 and sidv
                //     ldr v0, #2
                //     sdiv v2, v1, v0
                //     -----
                //     ldr v0, #2
                //     mov v2, v1, ASR#1
                //
                // this is found in performance/conv
                // occurs when there is a mod 2 operation
                // FIXME: not always correct
                // else if (curr_inst->isLoad() && next_inst->isDiv()) {
                //     auto maybe_imm = curr_inst->getUse()[0];
                //     if (!maybe_imm->isImm()) {
                //         continue;
                //     }
                //     auto ldr_dst = curr_inst->getDef()[0];
                //     auto div_src = next_inst->getUse()[1];
                //     if (ldr_dst->getReg() != div_src->getReg()) {
                //         continue;
                //     }
                //     if (maybe_imm->getVal() == 2) {
                //         auto new_inst = new MovMInstruction(
                //             block, MovMInstruction::MOVASR,
                //             next_inst->getDef()[0], next_inst->getUse()[0],
                //             MovMInstruction::NONE,
                //             new MachineOperand(MachineOperand::IMM, 1));
                //         *next_inst_iter = new_inst;
                //     }
                // }

                for (auto inst : instToRemove) {
                    block->remove(inst);
                }
            }
        }
    }
}