#include <set>
#include <vector>
#include "LiveVariableAnalysis.h"
#include "MachineDeadCodeElimination.h"

using namespace std;

// https://decaf-lang.gitbook.io/decaf-book/rust-kuang-jia-fen-jie-duan-zhi-dao/pa4-zhong-jian-dai-ma-you-hua/si-dai-ma-xiao-chu

void MachineDeadCodeElimination::pass() {
    for (auto func : unit->getFuncs())
        pass(func);
}

void MachineDeadCodeElimination::pass(MachineFunction* func) {
    LiveVariableAnalysis lva;
    lva.pass(func);
    map<MachineOperand, set<MachineOperand*>> out;
    vector<MachineInstruction*> temp;
    for (auto block : func->getBlocks()) {
        out.clear();
        for (auto& t : block->getLiveOut())
            out[*t].insert(t);
        for (auto ins = block->getInsts().rbegin();
             ins != block->getInsts().rend(); ins++) {
            auto defs = (*ins)->getDef();
            if (!defs.empty()) {
                // 除bl以外def应该都只是1个
                // bl也只有第一个r0有用
                MachineOperand* def = nullptr;
                if((*ins)->isSmull()){
                    def = defs[1];
                }
                else
                    def = defs[0];
                if (out[*def].empty())
                    temp.push_back(*ins);
            }
            for (auto& def : defs)
                for (auto& o : lva.getAllUses()[*def])
                    if (out[*def].find(o) != out[*def].end())
                        out[*def].erase(o);
            auto uses = (*ins)->getUse();
            for (auto& use : uses)
                out[*use].insert(use);
        }
    }
    for (auto t : temp) {
        if (t->isBL())
            continue;
        if (t->isCondMov()) {
            auto next = t->getParent()->getNext(t);
            if (next && next->isCondMov())
                continue;
        }
        t->getParent()->remove(t);
    }
}