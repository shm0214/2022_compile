#include "GraphColor.h"
#include <cmath>
#include <vector>
#include "ControlFlowAnalysis.h"
#include "LiveVariableAnalysis.h"
#include "ReachDefAnalysis.h"

#define MAX 1000000.0

using namespace std;

GraphColor::GraphColor(MachineUnit* unit) {
    this->unit = unit;
    regNum = 11;
    defWeight = 2;
    useWeight = 4;
    copyWeight = 1;
}

void GraphColor::allocateRegisters() {
    for (auto& func : unit->getFuncs()) {
        this->func = func;
        bool success = false;
        while (!success) {
            bool coalesce = true;
            while (coalesce) {
                makeWebs();
                makeMatrix();
                coalesce = coalesceRegs();
            }
            makeList();
            computeSpillCost();
            pruneGraph();
            success = assignRegs();
            if (success)
                modifyCode();
            else
                genSpillCode();
        }
    }
}

void GraphColor::makeDuChains() {
    ReachDefAnalysis rd;
    rd.pass(func);
    std::map<MachineOperand, std::set<MachineOperand*>> reachingDef;
    du_chains.clear();
    for (auto& bb : func->getBlocks()) {
        for (auto& inst : bb->getInsts()) {
            auto defs = inst->getDef();
            for (auto& def : defs)
                if (def->needColor())
                    du_chains[def].insert({});
        }
    }
    for (auto& bb : func->getBlocks()) {
        reachingDef.clear();
        for (auto& t : bb->getDefIn())
            reachingDef[*t].insert(t);
        for (auto& inst : bb->getInsts()) {
            for (auto& use : inst->getUse()) {
                if (use->needColor()) {
                    if (reachingDef[*use].empty())
                        du_chains[use].insert(use);
                    for (auto& def : reachingDef[*use])
                        du_chains[def].insert(use);
                }
            }
            auto defs = inst->getDef();
            for (auto& def : defs) {
                auto& t = reachingDef[*def];
                auto& s = rd.getAllDefs()[*def];
                std::set<MachineOperand*> res;
                set_difference(t.begin(), t.end(), s.begin(), s.end(),
                               inserter(res, res.end()));
                reachingDef[*def] = res;
                reachingDef[*def].insert(def);
            }
        }
    }

    // // 先用linearscan的试试
    // LiveVariableAnalysis lva;
    // lva.pass(func);
    // du_chains.clear();
    // int i = 0;
    // std::map<MachineOperand, std::set<MachineOperand*>> liveVar;
    // for (auto& bb : func->getBlocks()) {
    //     liveVar.clear();
    //     for (auto& t : bb->getLiveOut())
    //         liveVar[*t].insert(t);
    //     int no;
    //     no = i = bb->getInsts().size() + i;
    //     for (auto inst = bb->getInsts().rbegin(); inst != bb->getInsts().rend();
    //          inst++) {
    //         (*inst)->setNo(no--);
    //         for (auto& def : (*inst)->getDef()) {
    //             if (def->isVReg()) {
    //                 auto& uses = liveVar[*def];
    //                 du_chains[def].insert(uses.begin(), uses.end());
    //                 auto& kill = lva.getAllUses()[*def];
    //                 std::set<MachineOperand*> res;
    //                 set_difference(uses.begin(), uses.end(), kill.begin(),
    //                                kill.end(), inserter(res, res.end()));
    //                 liveVar[*def] = res;
    //             }
    //         }
    //         for (auto& use : (*inst)->getUse()) {
    //             if (use->isVReg())
    //                 liveVar[*use].insert(use);
    //         }
    //     }
    // }
}

void GraphColor::makeWebs() {
    makeDuChains();
    webs.clear();
    operand2web.clear();
    for (auto& du_chain : du_chains) {
        Web* web = new Web({{du_chain.first},
                            du_chain.second,
                            false,
                            du_chain.first->isReg() ? MAX / 2 : 0,
                            0,
                            -1,
                            -1});
        webs.push_back(web);
    }
    bool again = true;
    while (again) {
        again = false;
        vector<Web*> temp(webs.begin(), webs.end());
        for (size_t i = 0; i < temp.size(); i++)
            for (size_t j = i + 1; j < temp.size(); j++) {
                Web* w1 = temp[i];
                Web* w2 = temp[j];
                if (**w1->defs.begin() == **w2->defs.begin()) {
                    std::set<MachineOperand*> temp;
                    set_intersection(w1->uses.begin(), w1->uses.end(),
                                     w2->uses.begin(), w2->uses.end(),
                                     inserter(temp, temp.end()));
                    if (!temp.empty()) {
                        again = true;
                        w1->defs.insert(w2->defs.begin(), w2->defs.end());
                        w1->uses.insert(w2->uses.begin(), w2->uses.end());
                        auto it = std::find(webs.begin(), webs.end(), w2);
                        if (it != webs.end())
                            webs.erase(it);
                    }
                }
            }
        // while (!temp.empty()) {
        //     auto web = temp[0];
        //     temp.erase(temp.begin());
        //     vector<Web*> temp2(temp.begin(), temp.end());
        //     while (!temp2.empty()) {
        //         auto web2 = temp2[0];
        //         temp2.erase(temp2.begin());
        //         if (**web->defs.begin() == **web2->defs.begin()) {
        //             set<MachineOperand*> t;
        //             set_intersection(web->uses.begin(), web->uses.end(),
        //                              web2->uses.begin(), web2->uses.end(),
        //                              inserter(t, t.end()));
        //             if (!t.empty()) {
        //                 again = true;
        //                 web->defs.insert(web2->defs.begin(), web2->defs.end());
        //                 web->uses.insert(web2->uses.begin(), web2->uses.end());
        //                 auto it = find(webs.begin(), webs.end(), web2);
        //                 if (it != webs.end())
        //                     webs.erase(it);
        //             }
        //         }
        //     }
        // }
    }
    vector<Web*> symreg;
    int i = 0;
    for (; i < regNum; i++) {
        Web* web = new Web({set<MachineOperand*>(), set<MachineOperand*>(),
                            false, MAX / 2, -1, -1, i});
        symreg.push_back(web);
    }
    for (auto& web : webs) {
        web->sreg = i;
        for (auto& def : web->defs)
            operand2web[def] = i;
        for (auto& use : web->uses)
            operand2web[use] = i;
        i++;
    }
    webs.insert(webs.begin(), symreg.begin(), symreg.end());
}

void GraphColor::makeMatrix() {
    LiveVariableAnalysis lva;
    lva.pass(func);
    matrix.resize(webs.size());
    for (auto& v : matrix) {
        v.resize(webs.size());
        for (auto b : v)
            b = false;
    }
    for (int i = 0; i < regNum; i++)
        for (int j = 0; j < i; j++) {
            matrix[i][j] = true;
            matrix[j][i] = true;
        }
    for (auto i = regNum; i < (int)webs.size(); i++) {
        auto def = *(webs[i]->defs.begin());
        if (!def->isReg())
            continue;
        webs[i]->rreg = def->getReg();
        int k = operand2web[def];
        for (int i = 0; i < regNum; i++) {
            if (i == def->getReg())
                continue;
            matrix[k][i] = true;
            matrix[i][k] = true;
        }
    }
    for (auto& block : func->getBlocks()) {
        auto liveOut = block->getLiveOut();
        auto ins = block->getInsts();
        for (auto i = ins.rbegin(); i != ins.rend(); i++) {
            auto defs = (*i)->getDef();
            for (auto& def : defs) {
                if (operand2web.find(def) != operand2web.end()) {
                    int k = operand2web[def];
                    for (auto& live : liveOut) {
                        if (operand2web.find(live) == operand2web.end())
                            continue;
                        int j = operand2web[live];
                        matrix[k][j] = true;
                        matrix[j][k] = true;
                    }
                }
                auto& uses = lva.getAllUses()[*def];
                for (auto& use : uses)
                    if (liveOut.find(use) != liveOut.end())
                        liveOut.erase(use);
            }
            for (auto& use : (*i)->getUse())
                liveOut.insert(use);
        }
    }
}

void GraphColor::makeList() {
    list.resize(matrix.size(), vector<int>());
    for (auto i = 0; i < (int)matrix.size(); i++)
        for (auto j = 0; j < i; j++)
            if (matrix[i][j]) {
                list[i].push_back(j);
                list[j].push_back(i);
            }
    removeList = list;
}

bool GraphColor::coalesceRegs() {
    bool flag = false;
    vector<MachineInstruction*> tempList;
    for (auto& block : func->getBlocks()) {
        for (auto& ins : block->getInsts()) {
            bool flag1 = false;
            if (ins->isMov())
                flag1 = true;
            else if (ins->isAdd()) {
                auto uses = ins->getUse();
                for (auto use : uses)
                    if (use->isImm() && use->getVal() == 0)
                        flag1 = true;
            }
            if (!flag1)
                continue;
            auto def = *(ins->getDef().begin());
            auto uses = ins->getUse();
            if (uses.empty() || uses[0]->isImm())
                continue;
            auto use = uses[0];
            if (operand2web.find(def) == operand2web.end() ||
                operand2web.find(use) == operand2web.end())
                continue;
            if (def->isReg() || use->isReg())
                continue;
            int u = operand2web[def];
            int v = operand2web[use];
            if (!matrix[u][v]) {
                flag = true;
                for (auto i = 0; i < (int)matrix.size(); i++)
                    matrix[i][u] = matrix[i][v] = matrix[u][i] = matrix[v][i] =
                        matrix[u][i] || matrix[v][i];
                for (auto& u_ : webs[u]->uses) {
                    auto in = u_->getParent();
                    MachineOperand* new_ = new MachineOperand(*use);
                    operand2web[new_] = v;
                    webs[v]->uses.insert(new_);
                    in->replaceUse(u_, new_);
                }
                for (auto& d : webs[u]->defs) {
                    auto in = d->getParent();
                    MachineOperand* new_ = new MachineOperand(*use);
                    operand2web[new_] = v;
                    webs[v]->defs.insert(new_);
                    in->replaceDef(d, new_);
                }
            }
            tempList.push_back(ins);
        }
    }
    for (auto& ins : tempList)
        ins->getParent()->remove(ins);
    return flag;
}

void GraphColor::computeSpillCost() {
    ControlFlowAnalysis cfa;
    cfa.pass(func);
    for (auto& block : func->getBlocks()) {
        auto factor = pow(10, cfa.getLoopDepth(block));
        for (auto& ins : block->getInsts()) {
            auto defs = ins->getDef();
            auto uses = ins->getUse();
            bool flag = false;
            if (ins->isMov())
                flag = true;
            else if (ins->isAdd()) {
                auto uses = ins->getUse();
                for (auto& use : uses)
                    if (use->isImm() && use->getVal() == 0)
                        flag = true;
            }
            for (auto& def : defs) {
                if (def->isReg())
                    continue;
                int i = operand2web[def];
                webs[i]->spillCost += factor * defWeight;
                if (flag)
                    webs[i]->spillCost -= factor * copyWeight;
            }
            for (auto& use : uses) {
                if (use->isReg())
                    continue;
                int i = operand2web[use];
                webs[i]->spillCost += factor * useWeight;
                if (flag)
                    webs[i]->spillCost -= factor * copyWeight;
            }
        }
    }
}

void GraphColor::pruneGraph() {
    while (!stk.empty())
        stk.pop();
    for (int i = 0; i < (int)list.size(); i++)
        if (list[i].size() == 0)
            stk.push(i);
    bool success = true;
    while (success) {
        success = false;
        for (int i = 0; i < (int)list.size(); i++)
            if (list[i].size() > 0 && (int)list[i].size() < regNum &&
                webs[i]->rreg == -1) {
                success = true;
                stk.push(i);
                adjustNeighbours(i);
            }
    }
    while (stk.size() < list.size()) {
        double spillCost = MAX;
        int spillNode;
        for (int i = 0; i < (int)list.size(); i++) {
            int nInts = list[i].size();
            if (nInts > 0 && webs[i]->spillCost / nInts < spillCost) {
                spillNode = i;
                spillCost = webs[i]->spillCost / nInts;
            }
        }
        stk.push(spillNode);
        adjustNeighbours(spillNode);
    }
}

void GraphColor::adjustNeighbours(int i) {
    for (auto j : list[i]) {
        auto it = find(list[j].begin(), list[j].end(), i);
        if (it != list[j].end()) {
            list[j].erase(it);
        }
        if (list[j].empty())
            stk.push(j);
    }
    list[i].clear();
}

bool GraphColor::assignRegs() {
    bool success = true;
    while (!stk.empty()) {
        auto r = stk.top();
        stk.pop();
        int color = minColor(r);
        if (color > -1)
            webs[r]->rreg = color;
        else {
            webs[r]->spill = true;
            success = false;
        }
    }
    return success;
}

int GraphColor::minColor(int r) {
    // 返回与r相邻的节点中还未使用的颜色中编号最小的颜色编号
    vector<bool> use;
    use.resize(regNum);
    for (auto& i : removeList[r])
        if (webs[i]->rreg != -1)
            use[webs[i]->rreg] = true;
    for (int i = 0; i < regNum; i++)
        if (!use[i])
            return i;
    return -1;
}

void GraphColor::modifyCode() {
    for (int i = regNum; i < (int)webs.size(); i++) {
        auto web = webs[i];
        auto rreg = web->rreg;
        if (rreg > 3)
            func->addSavedRegs(rreg);
        for (auto def : web->defs)
            def->setReg(rreg);
        for (auto use : web->uses)
            use->setReg(rreg);
    }
    vector<MachineInstruction*> temp;
    for (auto& block : func->getBlocks()) {
        for (auto& ins : block->getInsts()) {
            bool flag = false;
            if (ins->isMov())
                flag = true;
            else if (ins->isAdd()) {
                auto uses = ins->getUse();
                for (auto use : uses)
                    if (use->isImm() && use->getVal() == 0)
                        flag = true;
            }
            if (!flag)
                continue;
            auto def = *(ins->getDef().begin());
            auto uses = ins->getUse();
            if (uses.empty() || uses[0]->isImm())
                continue;
            auto use = *(uses.begin());
            if (def->getReg() == use->getReg())
                temp.push_back(ins);
        }
    }
    for (auto& ins : temp)
        ins->getParent()->remove(ins);
}

void GraphColor::genSpillCode() {
    for (auto web : webs) {
        if (!web->spill)
            continue;
        web->disp = -func->AllocSpace(4);
        auto off = new MachineOperand(MachineOperand::IMM, web->disp);
        auto fp = new MachineOperand(MachineOperand::REG, 11);
        for (auto use : web->uses) {
            auto temp = new MachineOperand(*use);
            MachineOperand* operand = nullptr;
            if (web->disp > 255 || web->disp < -255) {
                operand = new MachineOperand(MachineOperand::VREG,
                                             SymbolTable::getLabel());
                auto inst1 = new LoadMInstruction(use->getParent()->getParent(),
                                                  operand, off);
                use->getParent()->insertBefore(inst1);
            }
            if (operand) {
                auto inst =
                    new LoadMInstruction(use->getParent()->getParent(), temp,
                                         fp, new MachineOperand(*operand));
                use->getParent()->insertBefore(inst);
            } else {
                auto inst = new LoadMInstruction(use->getParent()->getParent(),
                                                 temp, fp, off);
                use->getParent()->insertBefore(inst);
            }
        }
        for (auto def : web->defs) {
            if(!def->getParent())
                continue;
            auto temp = new MachineOperand(*def);
            MachineOperand* operand = nullptr;
            MachineInstruction *inst1 = nullptr, *inst = nullptr;
            if (web->disp > 255 || web->disp < -255) {
                operand = new MachineOperand(MachineOperand::VREG,
                                             SymbolTable::getLabel());
                inst1 = new LoadMInstruction(def->getParent()->getParent(),
                                             operand, off);
                def->getParent()->insertAfter(inst1);
            }
            if (operand)
                inst =
                    new StoreMInstruction(def->getParent()->getParent(), temp,
                                          fp, new MachineOperand(*operand));
            else
                inst = new StoreMInstruction(def->getParent()->getParent(),
                                             temp, fp, off);
            if (inst1)
                inst1->insertAfter(inst);
            else
                def->getParent()->insertAfter(inst);
        }
    }
}