#include "LocalValueNumber.h"
#include "MachineDeadCodeElimination.h"

using namespace std;

int log2(int value);
inline bool is2Exp(int val) {
    return !(val & (val - 1));
}

// 简单的局部（基本块内）值编号 主要删除数组相关的冗余

void LocalValueNumber::pass() {
    changeMul2Exp2LSL();
    MachineDeadCodeElimination mdce(unit);
    mdce.pass();
    for (auto func : unit->getFuncs())
        pass(func);
}

void LocalValueNumber::pass(MachineFunction* func) {
    for (auto block : func->getBlocks()) {
        pass(block);
    }
}

void LocalValueNumber::pass(MachineBlock* block) {
    map<string, MachineOperand> hashTable;
    map<MachineOperand, MachineOperand> valueNumber;
    vector<MachineInstruction*> rmvList;
    for (auto in : block->getInsts()) {
        auto uses = in->getUse();
        for (auto u : uses) {
            auto temp = u;
            while (valueNumber.count(*temp)) {
                auto t = valueNumber[*temp];
                temp = new MachineOperand(t);
            }
            in->replaceUse(u, temp);
        }
        auto hash = in->getHash();
        if (hash.size()) {
            auto dst = in->getDef()[0];
            if (hashTable.count(hash)) {
                valueNumber[*dst] = hashTable[hash];
                rmvList.push_back(in);
            } else {
                if (in->isMov() && uses[0]->isVReg())
                    valueNumber[*dst] = *(uses[0]);
                hashTable[hash] = *dst;
            }
        }
    }
    // for (auto in : rmvList) {
    //     in->getParent()->remove(in);
    // }
}

void LocalValueNumber::changeMul2Exp2LSL() {
    for (auto func_iter = unit->begin(); func_iter != unit->end();
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
            for (; next_inst_iter != block->end();
                 curr_inst_iter++, next_inst_iter++) {
                auto curr_inst = *curr_inst_iter;
                auto next_inst = *next_inst_iter;
                if (curr_inst->isMov() && next_inst->isMul()) {
                    auto movDst = curr_inst->getDef()[0];
                    auto movSrc = curr_inst->getUse()[0];
                    if (movSrc->isImm()) {
                        int val = movSrc->getVal();
                        if (val && !movSrc->isFloat() && is2Exp(val)) {
                            int log = log2(val);
                            auto mulDst = next_inst->getDef()[0];
                            auto mulSrc1 = next_inst->getUse()[0];
                            auto mulSrc2 = next_inst->getUse()[1];
                            MachineOperand* lslSrc = nullptr;
                            if (*mulSrc1 == *movDst)
                                lslSrc = mulSrc2;
                            if (*mulSrc2 == *movDst)
                                lslSrc = mulSrc1;
                            if (lslSrc) {
                                auto imm = new MachineOperand(
                                    MachineOperand::IMM, log);
                                auto lsl = new MovMInstruction(
                                    block, MovMInstruction::MOVLSL, mulDst,
                                    lslSrc, 6, imm);
                                *next_inst_iter = lsl;
                            }
                        }
                    }
                }
                if (curr_inst->isLoad() && next_inst->isMul()) {
                    auto ldrDst = curr_inst->getDef()[0];
                    auto srcs = curr_inst->getUse();
                    if (srcs.size() == 1) {
                        auto ldrSrc = srcs[0];
                        if (ldrSrc->isImm()) {
                            int val = ldrSrc->getVal();
                            if (val && !ldrSrc->isFloat() && is2Exp(val)) {
                                int log = log2(val);
                                auto mulDst = next_inst->getDef()[0];
                                auto mulSrc1 = next_inst->getUse()[0];
                                auto mulSrc2 = next_inst->getUse()[1];
                                MachineOperand* lslSrc = nullptr;
                                if (*mulSrc1 == *ldrDst)
                                    lslSrc = mulSrc2;
                                if (*mulSrc2 == *ldrDst)
                                    lslSrc = mulSrc1;
                                if (lslSrc) {
                                    auto imm = new MachineOperand(
                                        MachineOperand::IMM, log);
                                    auto lsl = new MovMInstruction(
                                        block, MovMInstruction::MOVLSL, mulDst,
                                        lslSrc, 6, imm);
                                    *next_inst_iter = lsl;
                                }
                            }
                        }
                    }
                }
                auto next_next_inst_iter = next_inst_iter + 1;
                if (next_next_inst_iter != block->end()) {
                    auto next_next_inst = *next_next_inst_iter;
                    if (curr_inst->isMov() && next_next_inst->isMul()) {
                        auto movDst = curr_inst->getDef()[0];
                        auto movSrc = curr_inst->getUse()[0];
                        if (movSrc->isImm()) {
                            int val = movSrc->getVal();
                            if (val && !movSrc->isFloat() && is2Exp(val)) {
                                int log = log2(val);
                                auto mulDst = next_next_inst->getDef()[0];
                                auto mulSrc1 = next_next_inst->getUse()[0];
                                auto mulSrc2 = next_next_inst->getUse()[1];
                                MachineOperand* lslSrc = nullptr;
                                if (*mulSrc1 == *movDst)
                                    lslSrc = mulSrc2;
                                if (*mulSrc2 == *movDst)
                                    lslSrc = mulSrc1;
                                if (lslSrc) {
                                    auto imm = new MachineOperand(
                                        MachineOperand::IMM, log);
                                    auto lsl = new MovMInstruction(
                                        block, MovMInstruction::MOVLSL, mulDst,
                                        lslSrc, 6, imm);
                                    *next_next_inst_iter = lsl;
                                }
                            }
                        }
                    }
                    if (curr_inst->isLoad() && next_next_inst->isMul()) {
                        auto ldrDst = curr_inst->getDef()[0];
                        auto srcs = curr_inst->getUse();
                        if (srcs.size() == 1) {
                            auto ldrSrc = srcs[0];
                            if (ldrSrc->isImm()) {
                                int val = ldrSrc->getVal();
                                if (val && !ldrSrc->isFloat() && is2Exp(val)) {
                                    int log = log2(val);
                                    auto mulDst = next_next_inst->getDef()[0];
                                    auto mulSrc1 = next_next_inst->getUse()[0];
                                    auto mulSrc2 = next_next_inst->getUse()[1];
                                    MachineOperand* lslSrc = nullptr;
                                    if (*mulSrc1 == *ldrDst)
                                        lslSrc = mulSrc2;
                                    if (*mulSrc2 == *ldrDst)
                                        lslSrc = mulSrc1;
                                    if (lslSrc) {
                                        auto imm = new MachineOperand(
                                            MachineOperand::IMM, log);
                                        auto lsl = new MovMInstruction(
                                            block, MovMInstruction::MOVLSL,
                                            mulDst, lslSrc, 6, imm);
                                        *next_next_inst_iter = lsl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}