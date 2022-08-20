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
            if (block->getInsts().empty()) {
                continue;
            }
            auto curr_inst_iter = block->begin();
            auto next_inst_iter = next(curr_inst_iter, 1);

            std::set<MachineInstruction*> instToRemove;

            for (; next_inst_iter != block->end();
                 curr_inst_iter++, next_inst_iter++) {
                auto curr_inst = *curr_inst_iter;
                auto next_inst = *next_inst_iter;
                if (curr_inst->isMov() || curr_inst->isVMovf32()) {
                    // mov r1, r1
                    // occurs after register allocation and peephole in
                    // performance/sort
                    auto dst = curr_inst->getDef()[0];
                    auto src = curr_inst->getUse()[0];
                    if (*dst == *src) {
                        instToRemove.insert(curr_inst);
                    }
                }

                if (curr_inst->isMul() && next_inst->isAdd()) {
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

                    auto mul_dst = curr_inst->getDef()[0];
                    auto add_src1 = next_inst->getUse()[0];
                    auto add_src2 = next_inst->getUse()[1];

                    auto rd = new MachineOperand(*(next_inst->getDef()[0]));
                    auto rn = new MachineOperand(*(curr_inst->getUse()[0]));
                    auto rm = new MachineOperand(*(curr_inst->getUse()[1]));
                    MachineOperand* ra;

                    // fix:  mul r0, r1, r0
                    //       add sp, sp, #0
                    if (add_src1->isImm() || add_src2->isImm())
                        continue;

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
                    // instToRemove.insert(curr_inst);

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
                    // instToRemove.insert(curr_inst);
                } else if (curr_inst->isVMul() && next_inst->isVAdd()) {
                    //
                    // vmul.f32 v0, v1, v2
                    // vadd.f32 v3, v4, v0
                    // -----
                    // vmla.f32 v4, v1, v2
                    // vmov.f32 v3, v4 // for dce

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

                    auto fused_inst = new FuseMInstruction(
                        block, FuseMInstruction::VMLA, ra, rn, rm, ra);

                    *curr_inst_iter = fused_inst;

                    auto vmov_inst = new MovMInstruction(
                        block, MovMInstruction::VMOVF32, rd, ra);

                    *next_inst_iter = vmov_inst;

                } else if (curr_inst->isVMul() && next_inst->isVSub()) {
                    
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
                        block, FuseMInstruction::VMLS, ra, rn, rm, ra);

                    *curr_inst_iter = fused_inst;

                    auto vmov_inst = new MovMInstruction(
                        block, MovMInstruction::VMOVF32, rd, ra);

                    *next_inst_iter = vmov_inst;

                } else if (curr_inst->isStore() && next_inst->isLoad()) {
                    // convert store and load into store and move
                    //     str v355, [v11]
                    //     ldr v227, [v11]
                    //     -----
                    //     str v355, [v11]
                    //     mov v227, v355

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
                    if (*str_stk_src1 == *ldr_stk_src1) {
                        if (str_stk_src2 == nullptr &&
                            ldr_stk_src2 == nullptr) {
                            dst = new MachineOperand(*dst);
                            src = new MachineOperand(*src);
                            MachineInstruction* new_inst;
                            if (dst->isFloat() && src->isFloat()) {
                                new_inst = new MovMInstruction(
                                    block, MovMInstruction::VMOVF32, dst, src);
                            } else {
                                new_inst = new MovMInstruction(
                                    block, MovMInstruction::MOV, dst, src);
                            }
                            // cannot determine whether the stack will be used
                            // again or not.
                            *next_inst_iter = new_inst;
                        } else if (str_stk_src2 != nullptr &&
                                   ldr_stk_src2 != nullptr &&
                                   *str_stk_src2 == *ldr_stk_src2) {
                            // for offset
                            dst = new MachineOperand(*dst);
                            src = new MachineOperand(*src);
                            MachineInstruction* new_inst;
                            if (dst->isFloat() && src->isFloat()) {
                                new_inst = new MovMInstruction(
                                    block, MovMInstruction::VMOVF32, dst, src);
                            } else {
                                new_inst = new MovMInstruction(
                                    block, MovMInstruction::MOV, dst, src);
                            }
                            // cannot determine whether the stack will be
                            // used again or not.
                            *next_inst_iter = new_inst;
                        }
                    }
                } else if (curr_inst->isLoad() && next_inst->isLoad()) {
                    // convert same loads to load and move
                    // 	   ldr r0, [fp, #-12]
                    //     ldr r1, [fp, #-12]
                    //     -----
                    // 	   ldr r0, [fp, #-12]
                    //     mov r1, r0
                    // in performance/fft

                    auto curr_src1 = curr_inst->getUse()[0];
                    MachineOperand* curr_src2 = nullptr;
                    if (curr_inst->getUse().size() > 1) {
                        curr_src2 = curr_inst->getUse()[1];
                    }
                    auto next_src1 = next_inst->getUse()[0];
                    MachineOperand* next_src2 = nullptr;
                    if (next_inst->getUse().size() > 1) {
                        next_src2 = next_inst->getUse()[1];
                    }

                    if (*curr_src1 == *next_src1 &&
                        ((curr_src2 == nullptr && next_src2 == nullptr) ||
                         (curr_src2 != nullptr && next_src2 != nullptr &&
                          *curr_src2 == *next_src2))) {
                        auto src = curr_inst->getDef()[0];
                        auto dst = next_inst->getDef()[0];
                        src = new MachineOperand(*src);
                        dst = new MachineOperand(*dst);
                        MachineInstruction* new_inst;
                        if (dst->isFloat() && src->isFloat()) {
                            new_inst = new MovMInstruction(
                                block, MovMInstruction::VMOVF32, dst, src);
                        } else {
                            new_inst = new MovMInstruction(
                                block, MovMInstruction::MOV, dst, src);
                        }
                        *next_inst_iter = new_inst;
                    }
                } else if (curr_inst->isMov() && next_inst->isAdd()) {
                    // array optimization after constant eval in asm
                    // 	   mov v1, #-120
                    //     add v0, fp, v1
                    //     -----
                    //     mov v1, #-120 (might be eliminated as dead code)
                    //     add v0, fp, #-120

                    auto mov_dst = curr_inst->getDef()[0];
                    auto mov_src = curr_inst->getUse()[0];

                    auto add_dst = next_inst->getDef()[0];
                    auto add_src1 = next_inst->getUse()[0];
                    auto add_src2 = next_inst->getUse()[1];

                    if (*mov_dst == *add_src2 && mov_src->isImm()) {
                        auto dst = new MachineOperand(*add_dst);
                        auto src1 = new MachineOperand(*add_src1);
                        auto src2 = new MachineOperand(*mov_src);
                        auto new_inst = new BinaryMInstruction(
                            block, BinaryMInstruction::ADD, dst, src1, src2);
                        *next_inst_iter = new_inst;
                    }
                } else if (curr_inst->isLoad() && next_inst->isVMov()) {
                    // ldr r4, =0
                    // vmov s5, r4
                    // vsub.f32 s7, s5, s6
                    // -----
                    // vneg.f32 s7, s6
                    if (curr_inst->getUse().size() != 1) {
                        continue;
                    }
                    auto src = curr_inst->getUse()[0];

                    if (!src->isImm()) {
                        continue;
                    }

                    if (src->getVal() != 0) {
                        continue;
                    }

                    if (next_inst_iter + 1 == block->end()) {
                        continue;
                    }

                    auto maybe_sub_inst = *(next_inst_iter + 1);

                    if (!maybe_sub_inst->isVSub()) {
                        continue;
                    }

                    auto ldr_dst = curr_inst->getDef()[0];
                    auto vmov_dst = next_inst->getDef()[0];
                    auto vmov_src = next_inst->getUse()[0];

                    if (ldr_dst->getReg() != vmov_src->getReg()) {
                        continue;
                    }

                    auto vsub_dst = maybe_sub_inst->getDef()[0];
                    auto vsub_src1 = maybe_sub_inst->getUse()[0];
                    auto vsub_src2 = maybe_sub_inst->getUse()[1];

                    if (vmov_dst->getReg() == vsub_src1->getReg()) {
                        auto dst = new MachineOperand(*vsub_dst);
                        auto src = new MachineOperand(*vsub_src2);
                        auto new_inst = new VNegMInstruction(
                            block, VNegMInstruction::F32, dst, src);

                        *(next_inst_iter + 1) = new_inst;
                        // instToRemove.insert(curr_inst);
                        // instToRemove.insert(next_inst);
                    }
                }
            }
            for (auto inst : instToRemove) {
                block->remove(inst);
            }
        }
        for (auto block_iter = func->begin(); block_iter != func->end();
             block_iter++) {
            auto block = *block_iter;
            if (block->getInsts().empty()) {
                continue;
            }
            auto curr_inst_iter = block->begin();
            auto next_inst_iter = next(curr_inst_iter, 1);

            std::set<MachineInstruction*> instToRemove;

            for (; next_inst_iter != block->end();
                 curr_inst_iter++, next_inst_iter++) {
                auto curr_inst = *curr_inst_iter;
                auto next_inst = *next_inst_iter;

                if (curr_inst->isAdd() && next_inst->isStore()) {
                    // array or overflow
                    //   add v11, fp, #-128
                    //   str v0, [v11]
                    //  -----
                    //   str v0, [fp, #-128]

                    if (next_inst->getUse().size() > 2)
                        continue;

                    auto add_dst = curr_inst->getDef()[0];
                    auto add_src1 = curr_inst->getUse()[0];
                    auto add_src2 = curr_inst->getUse()[1];

                    auto store_src1 = next_inst->getUse()[0];
                    auto store_src2 = next_inst->getUse()[1];

                    if (store_src1->isFloat() &&
                        !(add_src2->isImm() && add_src2->getVal() == 0))
                        continue;

                    // 这个dce消不掉了
                    if (add_dst->isReg())
                        continue;

                    if (*add_dst == *store_src2) {
                        auto src1 = new MachineOperand(*store_src1);
                        auto src2 = new MachineOperand(*add_src1);
                        auto src3 = new MachineOperand(*add_src2);
                        if (add_src2->isImm() && add_src2->getVal() == 0)
                            src3 = nullptr;
                        auto new_inst = new StoreMInstruction(
                            block, next_inst->getOp(), src1, src2, src3);
                        *next_inst_iter = new_inst;
                        if (add_dst->isReg())
                            instToRemove.insert(curr_inst);
                    }
                } else if (curr_inst->isAdd() && next_inst->isLoad()) {
                    // array or overflow
                    //   add v11, fp, #-128
                    //   ldr v0, [v11]
                    //  -----
                    //   ldr v0, [fp, #-128]

                    if (next_inst->getUse().size() > 1)
                        continue;

                    auto add_dst = curr_inst->getDef()[0];
                    auto add_src1 = curr_inst->getUse()[0];
                    auto add_src2 = curr_inst->getUse()[1];

                    auto load_dst = next_inst->getDef()[0];
                    auto load_src1 = next_inst->getUse()[0];

                    if (load_dst->isFloat() &&
                        !(add_src2->isImm() && add_src2->getVal() == 0))
                        continue;

                    if (add_dst->isReg())
                        continue;

                    if (*add_dst == *load_src1) {
                        auto src1 = new MachineOperand(*load_dst);
                        auto src2 = new MachineOperand(*add_src1);
                        auto src3 = new MachineOperand(*add_src2);
                        if (add_src2->isImm() && add_src2->getVal() == 0)
                            src3 = nullptr;
                        auto new_inst = new LoadMInstruction(
                            block, next_inst->getOp(), src1, src2, src3);
                        *next_inst_iter = new_inst;
                        if (add_dst->isReg() && *add_dst == *load_dst)
                            instToRemove.insert(curr_inst);
                    }
                }
            }
            for (auto inst : instToRemove) {
                block->remove(inst);
            }
        }
    }
}

void PeepholeOptimization::pass1() {
    for (auto func_iter = mUnit->begin(); func_iter != mUnit->end();
         func_iter++) {
        auto func = *func_iter;
        for (auto block_iter = func->begin(); block_iter != func->end();
             block_iter++) {
            auto block = *block_iter;
            if (block->getInsts().empty()) {
                continue;
            }
            auto curr_inst_iter = block->begin();
            auto next_inst_iter = next(curr_inst_iter, 1);

            std::set<MachineInstruction*> instToRemove;

            for (; next_inst_iter != block->end();
                 curr_inst_iter++, next_inst_iter++) {
                auto curr_inst = *curr_inst_iter;
                auto next_inst = *next_inst_iter;

                if (curr_inst->isLoad() && next_inst->isVMov()) {
                    // ldr r4, =0
                    // vmov s5, r4
                    // vsub.f32 s7, s5, s6
                    // -----
                    // vneg.f32 s7, s6
                    if (curr_inst->getUse().size() != 1) {
                        continue;
                    }
                    auto src = curr_inst->getUse()[0];

                    if (!src->isImm()) {
                        continue;
                    }

                    if (src->getVal() != 0) {
                        continue;
                    }

                    if (next_inst_iter + 1 == block->end()) {
                        continue;
                    }

                    auto maybe_sub_inst = *(next_inst_iter + 1);

                    if (!maybe_sub_inst->isVSub()) {
                        continue;
                    }

                    auto ldr_dst = curr_inst->getDef()[0];
                    auto vmov_dst = next_inst->getDef()[0];
                    auto vmov_src = next_inst->getUse()[0];

                    if (ldr_dst->getReg() != vmov_src->getReg()) {
                        continue;
                    }

                    auto vsub_dst = maybe_sub_inst->getDef()[0];
                    auto vsub_src1 = maybe_sub_inst->getUse()[0];
                    auto vsub_src2 = maybe_sub_inst->getUse()[1];

                    if (vmov_dst->getReg() == vsub_src1->getReg()) {
                        auto dst = new MachineOperand(*vsub_dst);
                        auto src = new MachineOperand(*vsub_src2);
                        auto new_inst = new VNegMInstruction(
                            block, VNegMInstruction::F32, dst, src);

                        *(next_inst_iter + 1) = new_inst;
                        instToRemove.insert(curr_inst);
                        instToRemove.insert(next_inst);
                    }
                }
            }
            for (auto inst : instToRemove) {
                block->remove(inst);
            }
        }
    }
}