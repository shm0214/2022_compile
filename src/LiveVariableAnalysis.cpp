#include "LiveVariableAnalysis.h"
#include <algorithm>
#include "MachineCode.h"

void LiveVariableAnalysis::pass(MachineUnit* unit) {
    for (auto& func : unit->getFuncs()) {
        computeUsePos(func);
        computeDefUse(func);
        iterate(func);
    }
}

void LiveVariableAnalysis::pass(MachineFunction* func) {
    computeUsePos(func);
    computeDefUse(func);
    iterate(func);
}

void LiveVariableAnalysis::computeDefUse(MachineFunction* func) {
    for (auto& block : func->getBlocks()) {
        for (auto inst = block->getInsts().begin();
             inst != block->getInsts().end(); inst++) {
            auto user = (*inst)->getUse();
            std::set<MachineOperand*> temp;
            for (auto use : user)
                if (use->isVReg() || use->isReg())
                    temp.insert(use);
            set_difference(temp.begin(), temp.end(), def[block].begin(),
                           def[block].end(),
                           inserter(use[block], use[block].end()));
            auto defs = (*inst)->getDef();
            // def应为首次出现即为定值的，就是之前不能有use的
            for (auto& d : defs) {
                // auto uses = all_uses[*d];
                // bool flag = true;
                // for (auto& u : uses) {
                //     if (u->getParent()->getParent() == block) {
                //         if (block->isBefore(u->getParent(), *inst)) {
                //             flag = false;
                //             break;
                //         }
                //     }
                // }
                // if (flag)
                def[block].insert(all_uses[*d].begin(), all_uses[*d].end());
            }
        }
    }
}

void LiveVariableAnalysis::iterate(MachineFunction* func) {
    for (auto& block : func->getBlocks())
        block->getLiveIn().clear();
    bool change;
    change = true;
    while (change) {
        change = false;
        for (auto& block : func->getBlocks()) {
            block->getLiveOut().clear();
            auto old = block->getLiveIn();
            // 跳过exit
            // 好像没有严格的entry与exit 如果有ret值的话这样反而错了
            // if(block->getSuccs().size() == 0)
            //     continue;
            for (auto& succ : block->getSuccs())
                block->getLiveOut().insert(succ->getLiveIn().begin(),
                                           succ->getLiveIn().end());
            block->getLiveIn() = use[block];
            std::vector<MachineOperand*> temp;
            set_difference(
                block->getLiveOut().begin(), block->getLiveOut().end(),
                def[block].begin(), def[block].end(),
                inserter(block->getLiveIn(), block->getLiveIn().end()));
            if (old != block->getLiveIn())
                change = true;
        }
    }
}

void LiveVariableAnalysis::computeUsePos(MachineFunction* func) {
    for (auto& block : func->getBlocks()) {
        for (auto& inst : block->getInsts()) {
            auto uses = inst->getUse();
            for (auto& use : uses)
                if (use->isVReg() || use->isReg())
                    all_uses[*use].insert(use);
        }
    }
}
