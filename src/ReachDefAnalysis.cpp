#include "ReachDefAnalysis.h"
#include <algorithm>
#include <set>
using namespace std;

void ReachDefAnalysis::pass(MachineUnit* unit) {
    for (auto& func : unit->getFuncs()) {
        computeDefPos(func);
        computeGenKill(func);
        iterate(func);
    }
}

void ReachDefAnalysis::pass(MachineFunction* func) {
    computeDefPos(func);
    computeGenKill(func);
    iterate(func);
}

void ReachDefAnalysis::computeGenKill(MachineFunction* func) {
    for (auto& block : func->getBlocks()) {
        for (auto ins = block->getInsts().rbegin();
             ins != block->getInsts().rend(); ins++) {
            auto defs = (*ins)->getDef();
            for (auto def : defs) {
                set<MachineOperand*> temp({def}), res;
                set_difference(temp.begin(), temp.end(), kill[block].begin(),
                               kill[block].end(),
                               inserter(gen[block], gen[block].end()));
                kill[block].insert(all_defs[*def].begin(),
                                   all_defs[*def].end());
                kill[block].erase(def);
            }
        }
    }
}

void ReachDefAnalysis::computeDefPos(MachineFunction* func) {
    for (auto& block : func->getBlocks())
        for (auto& ins : block->getInsts()) {
            auto defs = ins->getDef();
            for (auto& def : defs)
                all_defs[*def].insert(def);
        }
}

void ReachDefAnalysis::iterate(MachineFunction* func) {
    for (auto& block : func->getBlocks())
        block->getDefOut().clear();
    auto entry = func->getSymbolEntry();
    FunctionType* type = (FunctionType*)(entry->getType());
    if (!type->getParamsSe().empty()) {
        int i = 0;
        while (type->getParamsSe()[i] != nullptr && i < 4) {
            MachineOperand* param = new MachineOperand(MachineOperand::REG, i);
            func->getEntry()->getDefOut().insert(param);
            i++;
        }
    }
    func->getEntry()->getDefIn().clear();
    func->getEntry()->getDefOut().insert(gen[func->getEntry()].begin(),
                                        gen[func->getEntry()].end());
    bool change;
    change = true;
    while (change) {
        change = false;
        for (auto& block : func->getBlocks()) {
            if (block == func->getEntry())
                continue;
            block->getDefIn().clear();
            std::set<MachineOperand*> old = block->getDefOut();
            for (auto& pred : block->getPreds())
                block->getDefIn().insert(pred->getDefOut().begin(),
                                           pred->getDefOut().end());
            block->getDefOut() = gen[block];
            std::vector<MachineOperand*> temp;
            set_difference(block->getDefIn().begin(),
                           block->getDefIn().end(), kill[block].begin(),
                           kill[block].end(), inserter(temp, temp.end()));
            block->getDefOut().insert(temp.begin(), temp.end());
            if (old != block->getDefOut())
                change = true;
        }
    }
}
