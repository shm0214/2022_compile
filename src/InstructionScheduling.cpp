#include "InstructionScheduling.h"

InstructionScheduling::InstructionScheduling(MachineUnit* mUnit) {
    this->mUnit = mUnit;
}

void InstructionScheduling::pass() {
    for (auto func_iter = mUnit->begin(); func_iter != mUnit->end();
         func_iter++) {
        auto func = *func_iter;
        for (auto block_iter = func->begin(); block_iter != func->end();
             block_iter++) {
            block = *block_iter;
            init();
            if (insts.size() <= 1) {
                continue;
            }
            build();
            compute_priority();
            schedule();
            modify();
        }
    }
}

void InstructionScheduling::init() {
    auto inst_end = block->nonbranch_end();
    if (inst_end == block->getInsts().end()) {
        insts.clear();
        return;
    }
    insts =
        std::vector<MachineInstruction*>(block->getInsts().begin(), inst_end);
    if (insts.size() <= 1) {
        return;
    }
    dep.resize(insts.size());
    for (auto& it : dep) {
        it.resize(insts.size());
        for (size_t i = 0; i < it.size(); i++) {
            it[i] = 0;
        }
    }

    anti.resize(insts.size());
    for (auto& it : anti) {
        it.resize(insts.size());
        for (size_t i = 0; i < it.size(); i++) {
            it[i] = 0;
        }
    }

    ready.clear();
    active.clear();

    succs.resize(insts.size());
    preds.resize(insts.size());
    for (auto& it : succs) {
        it.clear();
    }
    for (auto& it : preds) {
        it.clear();
    }
    degree.resize(insts.size());
    for (auto& it : degree) {
        it = 0;
    }
    priority.resize(insts.size());
    for (auto& it : priority) {
        it = -1;
    }
    time.resize(insts.size());
    for (auto& it : time) {
        it = 0;
    }
}

void InstructionScheduling::build() {
    for (size_t i = 0; i < insts.size(); i++) {
        int latency = insts[i]->latency();
        if (insts[i]->isBranch()) {
            for (size_t j = i + 1; j < insts.size(); j++) {
                dep[i][j] = latency;
            }
        }

        for (size_t j = i + 1; j < insts.size(); j++) {
            auto prev_defs = insts[i]->getDef();
            auto prev_uses = insts[i]->getUse();
            auto curr_defs = insts[j]->getDef();
            auto curr_uses = insts[j]->getUse();

            for (auto prev_def : prev_defs) {
                for (auto curr_use : curr_uses) {
                    // dependence
                    // R-A-W
                    if (*prev_def == *curr_use) {
                        dep[i][j] = latency;
                    }
                }
            }

            for (auto prev_use : prev_uses) {
                for (auto curr_def : curr_defs) {
                    // anti edges
                    // W-A-R
                    if (*prev_use == *curr_def) {
                        anti[i][j] = latency;
                    }
                }
            }
        }
    }
    for (size_t i = 0; i < insts.size(); i++) {
        for (size_t j = 0; j < insts.size(); j++) {
            if (dep[i][j] != 0) {
                succs[i].push_back(j);
                preds[j].push_back(i);
                degree[j]++;
            }
        }
    }
}

void InstructionScheduling::compute_priority_helper(int x) {
    if (preds[x].empty()) {
        priority[x] = 0;
        return;
    }
    for (auto i : preds[x]) {
        if (priority[i] == -1) {
            compute_priority_helper(i);
        }
        priority[x] = std::max(priority[x], priority[i] + dep[i][x]);
    }
}

void InstructionScheduling::compute_priority() {
    for (size_t i = 0; i < priority.size(); i++) {
        if (priority[i] == -1) {
            compute_priority_helper(i);
        }
    }
}

void InstructionScheduling::schedule() {
    // https://www.inf.ed.ac.uk/teaching/courses/copt/lecture-6.pdf
    // http://www.cs.cmu.edu/afs/cs/academic/class/15745-f03/public/lectures/L16_handouts.pdf
    int cycles = 0;
    sched.clear();
    auto cmp = [&](int x, int y) -> bool { return priority[x] > priority[y]; };
    for (size_t i = 0; i < preds.size(); i++) {
        if (preds[i].empty()) {
            ready.insert(std::lower_bound(ready.begin(), ready.end(), i, cmp),
                         i);
        }
    }
    active.clear();
    while (!ready.empty() && !active.empty()) {
        std::set<int> toInsert;
        for (auto it = ready.begin(); it != ready.end();) {
            sched.push_back(*it);
            time[*it] = cycles;
            active.push_back(*it);

            if (!anti[*it].empty()) {
                for (auto i : anti[*it]) {
                    toInsert.insert(i);
                }
            }

            it = ready.erase(it);
        }

        for (auto i : toInsert) {
            ready.insert(std::lower_bound(ready.begin(), ready.end(), i, cmp),
                         i);
        }
        cycles++;

        for (auto it = active.begin(); it != active.end();) {
            if (time[*it] + insts[*it]->latency() <= cycles) {
                for (auto i : succs[*it]) {
                    degree[i]--;
                    if (degree[i] == 0) {
                        ready.insert(std::lower_bound(ready.begin(),
                                                      ready.end(), i, cmp),
                                     i);
                    }
                }
                it = active.erase(it);
            } else {
                it++;
            }
        }
    }
}

void InstructionScheduling::modify() {
    std::vector<MachineInstruction*> new_insts;
    for (auto i : sched) {
        new_insts.push_back(insts[i]);
    }
    block->getInsts() = new_insts;
}