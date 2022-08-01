#include "CleanAsmAddZero.h"
#include <vector>
#include "LiveVariableAnalysis.h"
using namespace std;

void CleanAsmAddZero::pass() {
    for (auto func : unit->getFuncs())
        pass(func);
}

void CleanAsmAddZero::pass(MachineFunction* func) {
    vector<MachineInstruction*> delList;
    map<MachineInstruction*, vector<MachineInstruction*>> loads;
    LiveVariableAnalysis lva;
    lva.pass(func);
    auto allUses = lva.getAllUses();
    for (auto block : func->getBlocks()) {
        for (auto in : block->getInsts()) {
            if (in->isAddZero()) {
                auto dst = in->getDef()[0];
                auto src = in->getUse()[0];
                if (src->isParam() && dst->isVReg()) {
                    auto uses = allUses[*dst];
                    // 只处理use一次的吧 否则多load一次还不如add一下
                    // 如果用的不是r3可能可以调整一下减少溢出 但用r3就很难了
                    if (uses.size() > 1)
                        continue;
                    for (auto use : uses) {
                        auto useIn = use->getParent();
                        MachineOperand* loadOpe;
                        MachineOperand* useOpe;
                        // if (useIn->isPush()) {
                        loadOpe = new MachineOperand(MachineOperand::VREG,
                                                     SymbolTable::getLabel());
                        useOpe = new MachineOperand(*loadOpe);
                        // } else {
                        //     loadOpe =
                        //         new MachineOperand(MachineOperand::REG, 3);
                        //     useOpe = new MachineOperand(MachineOperand::REG, 3);
                        // }
                        // 需要加一条load
                        auto fp = new MachineOperand(MachineOperand::REG, 11);
                        auto off = new MachineOperand(MachineOperand::IMM,
                                                      src->getOffset());
                        auto load = new LoadMInstruction(useIn->getParent(),
                                                         LoadMInstruction::LDR,
                                                         loadOpe, fp, off);
                        load->setNeedModify();
                        if (loads.count(useIn))
                            loads[useIn].push_back(load);
                        else
                            loads.insert({useIn, {load}});
                        use->getParent()->replaceUse(use, useOpe);
                    }
                    delList.push_back(in);
                }
                // // add sp, sp #0 后续可能不是0了 不能删
                // else if (dst->isReg() && src->isReg() && dst->getReg() == 13 &&
                //          src->getReg() == 13)
                //     continue;
                // // r0-r3的拷贝 寄存器分配会删 也不能删
                // else if (dst->isVReg() && src->isReg() && src->getReg() <= 3)
                //     continue;
                // // phi消除得到的 删除会影响正确性 不能删
                // else if (dst->isVReg() && src->isVReg())
                //     continue;
                // else {
                //     assert(0);
                // }
            }
        }
    }
    for (auto iter : loads) {
        auto in = iter.first;
        for (auto in1 : iter.second)
            in->getParent()->insertBefore(in1, in);
    }
    for (auto in : delList)
        in->getParent()->remove(in);
}
