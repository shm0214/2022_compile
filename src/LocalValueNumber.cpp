#include "LocalValueNumber.h"

using namespace std;

// 简单的局部（基本块内）值编号 主要删除数组相关的冗余

void LocalValueNumber::pass() {
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