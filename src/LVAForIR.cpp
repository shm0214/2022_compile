#include "LVAForIR.h"

// ref: engineering a compiler 8.6.1

using namespace std;

void LVAForIR::pass(Function* func) {
    for (auto block : func->getBlockList()) {
        init(block);
        computeLiveOut(func);
    }
}

void LVAForIR::init(BasicBlock* block) {
    UEVar[block] = {};
    VarKill[block] = {};
    for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
        auto def = in->getDef();
        auto uses = in->getUse();
        for (auto use : uses) {
            if (use)
                if (!VarKill[block].count(use))
                    UEVar[block].insert(use);
        }
        if (def)
            VarKill[block].insert(def);
    }
}

void LVAForIR::computeLiveOut(Function* func) {
    for (auto block : func->getBlockList())
        LiveOut[block] = {};
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto block : func->getBlockList()) {
            auto old = LiveOut[block];
            LiveOut[block].clear();
            for (auto& succ : block->getSucc()) {
                set<Operand*> temp = {};
                set_difference(LiveOut[succ].begin(), LiveOut[succ].end(),
                               VarKill[succ].begin(), VarKill[succ].end(),
                               inserter(temp, temp.end()));
                LiveOut[block].insert(temp.begin(), temp.end());
                LiveOut[block].insert(UEVar[succ].begin(), UEVar[succ].end());
            }
            if (old != LiveOut[block])
                changed = true;
        }
    }
}