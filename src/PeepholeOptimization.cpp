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

                if (curr_inst->isMul() && next_inst->isAdd()) {
                    auto mul_dst = curr_inst->getDef()[0];
                    auto add_src = next_inst->getUse()[1];
                    if (mul_dst != add_src) {
                        continue;
                    }

                    auto rd = next_inst->getDef()[0];
                    auto rn = curr_inst->getUse()[0];
                    auto rm = curr_inst->getUse()[1];
                    auto ra = next_inst->getUse()[0];
                    auto fused_inst = new FuseMInstruction(
                        block, FuseMInstruction::MLA, rd, rn, rm, ra);
                    *next_inst_iter = fused_inst;
                    instToRemove.insert(curr_inst);
                } else if (curr_inst->isMul() && next_inst->isSub()) {
                    auto mul_dst = curr_inst->getDef()[0];
                    auto sub_src = next_inst->getUse()[1];
                    if (mul_dst != sub_src) {
                        continue;
                    }
                    auto rd = next_inst->getDef()[0];
                    auto rn = curr_inst->getUse()[0];
                    auto rm = curr_inst->getUse()[1];
                    auto ra = next_inst->getUse()[0];
                    auto fused_inst = new FuseMInstruction(
                        block, FuseMInstruction::MLS, rd, rn, rm, ra);
                    *next_inst_iter = fused_inst;
                    instToRemove.insert(curr_inst);
                }
            }

            for (auto inst : instToRemove) {
                block->remove(inst);
            }
        }
    }
}