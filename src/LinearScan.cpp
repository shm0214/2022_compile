#include "LinearScan.h"
#include <algorithm>
#include <iostream>
#include "LiveVariableAnalysis.h"
#include "MachineCode.h"

LinearScan::LinearScan(MachineUnit* unit) {
    this->unit = unit;
    for (int i = 4; i < 11; i++) {
        regs.push_back(i);
    }
    for (int i = 4; i < 32; i++) {
        fpregs.push_back(i + 16);
    }
}

void LinearScan::allocateRegisters() {
    for (auto& f : unit->getFuncs()) {
        func = f;
        bool success;
        success = false;
        while (!success) {  // repeat until all vregs can be mapped
            computeLiveIntervals();
            success = linearScanRegisterAllocation();
            if (success)  // all vregs can be mapped to real regs
                modifyCode();
            else  // spill vregs that can't be mapped to real regs
                genSpillCode();
        }
    }
}

void LinearScan::makeDuChains() {
    LiveVariableAnalysis lva;
    lva.pass(func);
    du_chains.clear();
    int i = 0;
    std::map<MachineOperand, std::set<MachineOperand*>> liveVar;
    for (auto& bb : func->getBlocks()) {
        liveVar.clear();
        for (auto& t : bb->getLiveOut())
            liveVar[*t].insert(t);
        int no;
        no = i = bb->getInsts().size() + i;
        for (auto inst = bb->getInsts().rbegin(); inst != bb->getInsts().rend();
             inst++) {
            (*inst)->setNo(no--);
            for (auto& def : (*inst)->getDef()) {
                if (def->isVReg()) {
                    auto& uses = liveVar[*def];
                    du_chains[def].insert(uses.begin(), uses.end());
                    auto& kill = lva.getAllUses()[*def];
                    std::set<MachineOperand*> res;
                    std::set_difference(uses.begin(), uses.end(), kill.begin(),
                                        kill.end(),
                                        std::inserter(res, res.end()));
                    liveVar[*def] = res;
                }
            }
            for (auto& use : (*inst)->getUse()) {
                if (use->isVReg())
                    liveVar[*use].insert(use);
            }
        }
    }
}

void LinearScan::computeLiveIntervals() {
    makeDuChains();
    intervals.clear();
    for (auto& du_chain : du_chains) {
        int t = -1;
        for (auto& use : du_chain.second)
            t = std::max(t, use->getParent()->getNo());
        Interval* interval = new Interval({du_chain.first->getParent()->getNo(),
                                           t,
                                           false,
                                           0,
                                           0,
                                           du_chain.first->isFloat(),
                                           {du_chain.first},
                                           du_chain.second});
        intervals.push_back(interval);
    }
    bool change;
    change = true;
    while (change) {
        change = false;
        std::vector<Interval*> t(intervals.begin(), intervals.end());
        for (size_t i = 0; i < t.size(); i++)
            for (size_t j = i + 1; j < t.size(); j++) {
                Interval* w1 = t[i];
                Interval* w2 = t[j];
                if (**w1->defs.begin() == **w2->defs.begin()) {
                    std::set<MachineOperand*> temp;
                    set_intersection(w1->uses.begin(), w1->uses.end(),
                                     w2->uses.begin(), w2->uses.end(),
                                     inserter(temp, temp.end()));
                    if (!temp.empty()) {
                        change = true;
                        w1->defs.insert(w2->defs.begin(), w2->defs.end());
                        w1->uses.insert(w2->uses.begin(), w2->uses.end());
                        w1->start = std::min(w1->start, w2->start);
                        w1->end = std::max(w1->end, w2->end);
                        auto it =
                            std::find(intervals.begin(), intervals.end(), w2);
                        if (it != intervals.end())
                            intervals.erase(it);
                    }
                }
            }
    }
    sort(intervals.begin(), intervals.end(), compareStart);
}

bool LinearScan::linearScanRegisterAllocation() {
    bool success = true;
    active.clear();
    regs.clear();
    for (int i = 4; i < 11; i++)
        regs.push_back(i);
    for (int i = 4; i < 32; i++) {
        fpregs.push_back(i + 16);
    }
    for (auto& i : intervals) {
        expireOldIntervals(i);
        if ((!i->fpu && regs.empty()) || (i->fpu && fpregs.empty())) {
            spillAtInterval(i);
            // 不知道是不是该这样
            success = false;
        } else {
            if (!i->fpu) {
                i->rreg = regs.front();
                regs.erase(regs.begin());
            } else {
                i->rreg = fpregs.front();
                fpregs.erase(fpregs.begin());
            }
            active.push_back(i);
            sort(active.begin(), active.end(), compareEnd);
        }
    }
    return success;
}

void LinearScan::modifyCode() {
    for (auto& interval : intervals) {
        func->addSavedRegs(interval->rreg);
        for (auto def : interval->defs)
            def->setReg(interval->rreg);
        for (auto use : interval->uses)
            use->setReg(interval->rreg);
    }
}

void LinearScan::genSpillCode() {
    for (auto& interval : intervals) {
        if (!interval->spill)
            continue;
        // TODO: Verify for floating-point
        /* HINT:
         * The vreg should be spilled to memory.
         * 1. insert ldr inst before the use of vreg
         * 2. insert str inst after the def of vreg
         */
        interval->disp = -func->AllocSpace(4);
        auto off = new MachineOperand(MachineOperand::IMM, interval->disp);
        auto fp = new MachineOperand(MachineOperand::REG, 11);
        for (auto use : interval->uses) {
            auto temp = new MachineOperand(*use);
            MachineOperand* operand = nullptr;
            if (interval->disp > 255 || interval->disp < -255) {
                if (!use->isFloat()) {
                    operand = new MachineOperand(MachineOperand::VREG,
                                                 SymbolTable::getLabel());
                    auto inst1 = new LoadMInstruction(
                        use->getParent()->getParent(), LoadMInstruction::LDR,
                        operand, off);
                    use->getParent()->insertBefore(inst1);

                } else {
                    operand = new MachineOperand(MachineOperand::VREG,
                                                 SymbolTable::getLabel(), true);
                    auto inst1 = new LoadMInstruction(
                        use->getParent()->getParent(), LoadMInstruction::VLDR,
                        operand, off);
                    use->getParent()->insertBefore(inst1);
                }
            }
            if (operand) {
                if (!use->isFloat()) {
                    auto inst = new LoadMInstruction(
                        use->getParent()->getParent(), LoadMInstruction::LDR,
                        temp, fp, new MachineOperand(*operand));
                    use->getParent()->insertBefore(inst);

                } else {
                    auto inst = new LoadMInstruction(
                        use->getParent()->getParent(), LoadMInstruction::VLDR,
                        temp, fp, new MachineOperand(*operand));
                    use->getParent()->insertBefore(inst);
                }
            } else {
                if (!use->isFloat()) {
                    auto inst = new LoadMInstruction(
                        use->getParent()->getParent(), LoadMInstruction::LDR,
                        temp, fp, off);
                    use->getParent()->insertBefore(inst);

                } else {
                    auto inst = new LoadMInstruction(
                        use->getParent()->getParent(), LoadMInstruction::VLDR,
                        temp, fp, off);
                    use->getParent()->insertBefore(inst);
                }
            }
        }
        for (auto def : interval->defs) {
            auto temp = new MachineOperand(*def);
            MachineOperand* operand = nullptr;
            MachineInstruction *inst1 = nullptr, *inst = nullptr;
            if (interval->disp > 255 || interval->disp < -255) {
                if (!def->isFloat()) {
                    operand = new MachineOperand(MachineOperand::VREG,
                                                 SymbolTable::getLabel());
                    inst1 = new LoadMInstruction(def->getParent()->getParent(),
                                                 LoadMInstruction::LDR, operand,
                                                 off);
                    def->getParent()->insertAfter(inst1);

                } else {
                    operand = new MachineOperand(MachineOperand::VREG,
                                                 SymbolTable::getLabel(), true);
                    inst1 = new LoadMInstruction(def->getParent()->getParent(),
                                                 LoadMInstruction::VLDR,
                                                 operand, off);
                    def->getParent()->insertAfter(inst1);
                }
            }
            if (operand) {
                if (!def->isFloat()) {
                    inst = new StoreMInstruction(
                        def->getParent()->getParent(), StoreMInstruction::STR,
                        temp, fp, new MachineOperand(*operand));
                } else {
                    inst = new StoreMInstruction(
                        def->getParent()->getParent(), StoreMInstruction::VSTR,
                        temp, fp, new MachineOperand(*operand));
                }
            } else {
                if (!def->isFloat()) {
                    inst = new StoreMInstruction(def->getParent()->getParent(),
                                                 StoreMInstruction::STR, temp,
                                                 fp, off);
                } else {
                    inst = new StoreMInstruction(def->getParent()->getParent(),
                                                 StoreMInstruction::VSTR, temp,
                                                 fp, off);
                }
            }
            if (inst1)
                inst1->insertAfter(inst);
            else
                def->getParent()->insertAfter(inst);
        }
    }
}

void LinearScan::expireOldIntervals(Interval* interval) {
    auto it = active.begin();
    while (it != active.end()) {
        if ((*it)->end >= interval->start)
            return;
        if ((*it)->rreg < 11) {
            // general purpose registers
            regs.push_back((*it)->rreg);
            it = active.erase(find(active.begin(), active.end(), *it));
            sort(regs.begin(), regs.end());
        } else if ((*it)->rreg >= 20 && (*it)->rreg < 48) {
            // floating-point registers
            fpregs.push_back((*it)->rreg);
            it = active.erase(find(active.begin(), active.end(), *it));
            sort(fpregs.begin(), fpregs.end());

        } else {
            // error
        }
    }
}

void LinearScan::spillAtInterval(Interval* interval) {
    auto spill = active.back();
    if (spill->end > interval->end) {
        spill->spill = true;
        interval->rreg = spill->rreg;
        active.push_back(interval);
        sort(active.begin(), active.end(), compareEnd);
    } else {
        interval->spill = true;
    }
}

bool LinearScan::compareStart(Interval* a, Interval* b) {
    return a->start < b->start;
}

bool LinearScan::compareEnd(Interval* a, Interval* b) {
    return a->end < b->end;
}