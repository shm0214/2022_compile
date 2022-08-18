#include "GraphColor.h"
#include <cmath>
#include <vector>
#include "ControlFlowAnalysis.h"
#include "LiveVariableAnalysis.h"
#include "ReachDefAnalysis.h"

using namespace std;

GraphColor::GraphColor(MachineUnit* unit) {
    this->unit = unit;
    regNum = 11;
    fRegNum = 32;
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
            // func->output();
        }
    }
}

void GraphColor::makeDuChains() {
    ReachDefAnalysis rd;
    rd.pass(func);
    std::map<MachineOperand, std::set<MachineOperand*>> reachingDef;
    du_chains.clear();
    for (auto& block : func->getBlocks()) {
        for (auto& inst : block->getInsts()) {
            auto defs = inst->getDef();
            for (auto& def : defs)
                if (def->needColor())
                    du_chains[def].insert({});
        }
    }
    for (auto& block : func->getBlocks()) {
        reachingDef.clear();
        for (auto& t : block->getDefIn())
            reachingDef[*t].insert(t);
        for (auto& inst : block->getInsts()) {
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
    // for (auto& block : func->getBlocks()) {
    //     liveVar.clear();
    //     for (auto& t : block->getLiveOut())
    //         liveVar[*t].insert(t);
    //     int no;
    //     no = i = block->getInsts().size() + i;
    //     for (auto inst = block->getInsts().rbegin(); inst !=
    //     block->getInsts().rend();
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
                            du_chain.first->isReg() ? __DBL_MAX__ / 2 : 0,
                            0,
                            -1,
                            -1,
                            du_chain.first->isFloat()});
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
        //                 web->defs.insert(web2->defs.begin(),
        //                 web2->defs.end());
        //                 web->uses.insert(web2->uses.begin(),
        //                 web2->uses.end()); auto it = find(webs.begin(),
        //                 webs.end(), web2); if (it != webs.end())
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
                            false, __DBL_MAX__ / 2, -1, -1, i, false});
        symreg.push_back(web);
    }
    for (; i < regNum + fRegNum; i++) {
        Web* web = new Web({set<MachineOperand*>(), set<MachineOperand*>(),
                            false, __DBL_MAX__ / 2, -1, -1, i, true});
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
    for (int i = 0; i < regNum + fRegNum; i++)
        for (int j = 0; j < i; j++) {
            matrix[i][j] = true;
            matrix[j][i] = true;
        }
    for (auto i = regNum + fRegNum; i < (int)webs.size(); i++) {
        auto def = *(webs[i]->defs.begin());
        if (!def->isReg())
            continue;
        webs[i]->rreg = def->getReg();
        if (def->isFloat())
            webs[i]->rreg -= 5;
        auto rreg = webs[i]->rreg;
        int k = operand2web[def];
        for (int i = 0; i < regNum + fRegNum; i++) {
            if (i == rreg)
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
    // for (auto it = webs.begin() + regNum + fRegNum; it != webs.end(); it++) {
    //     auto web = *it;
    //     auto def = *(web->defs.begin());
    //     auto j = operand2web[def];
    //     if (web->fpu)
    //         for (int i = 0; i < regNum; i++)
    //             matrix[i][j] = matrix[j][i] = true;
    //     else
    //         for (int i = regNum; i < regNum + fRegNum; i++)
    //             matrix[i][j] = matrix[j][i] = true;
    // }
    for (auto it = webs.begin() + regNum + fRegNum; it != webs.end(); it++) {
        auto web = *it;
        auto def = *(web->defs.begin());
        auto j = operand2web[def];
        if (web->fpu) {
            for (int i = regNum + 12; i <= regNum + 15; i++)
                matrix[i][j] = matrix[j][i] = true;

            // matrix[regNum + 4][j] = matrix[j][regNum + 4] = true;
        }
    }
    // 虚的之间应该不用标记冲突吧
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
            else if (ins->isVMov()) {
                auto def = ins->getDef()[0];
                auto use = ins->getUse()[0];
                if (def->isFloat() && use->isFloat())
                    flag1 = true;
            } else if (ins->isAdd() || ins->isVAdd()) {
                auto def = ins->getDef()[0];
                if (def->isReg() && def->getReg() == 13)
                    continue;
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
            if (webs[u]->defs.size() != 1)
                continue;
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
                tempList.push_back(ins);
            }
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
            else if (ins->isVMov()) {
                auto def = ins->getDef()[0];
                auto use = ins->getUse()[0];
                if (def->isFloat() && use->isFloat())
                    flag = true;
            } else if (ins->isAdd() || ins->isVAdd()) {
                auto def = ins->getDef()[0];
                if (def->isReg() && def->getReg() == 13)
                    continue;
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
            if (list[i].size() > 0 && webs[i]->rreg == -1) {
                bool flag = false;
                if (webs[i]->fpu)
                    flag = (int)list[i].size() < fRegNum;
                else
                    flag = (int)list[i].size() < regNum;
                if (flag) {
                    success = true;
                    stk.push(i);
                    adjustNeighbours(i);
                }
            }
    }
    while (stk.size() < list.size()) {
        double spillCost = __DBL_MAX__;
        int spillNode = -1;
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
    vector<bool> use(regNum + fRegNum, false);
    bool fpu = webs[r]->fpu;
    for (auto& i : removeList[r])
        if (webs[i]->rreg != -1)
            use[webs[i]->rreg] = true;
    if (!fpu) {
        for (int i = 0; i < regNum; i++)
            if (!use[i])
                return i;
    } else {
        for (int i = regNum; i < regNum + fRegNum; i++)
            if (!use[i])
                return i;
    }
    return -1;
}

void GraphColor::modifyCode() {
    for (int i = regNum; i < (int)webs.size(); i++) {
        auto web = webs[i];
        auto rreg = web->rreg;
        if (web->fpu) {
            rreg = rreg - regNum + 16;
        }
        if (web->defs.size() || web->uses.size())
            if (rreg > 3 || rreg > 3 + 16)
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
            else if (ins->isVMov()) {
                auto def = ins->getDef()[0];
                auto use = ins->getUse()[0];
                if (def->isFloat() && use->isFloat())
                    flag = true;
            } else if (ins->isAdd() || ins->isVAdd()) {
                auto def = ins->getDef()[0];
                if (def->isReg() && def->getReg() == 13)
                    continue;
                auto uses = ins->getUse();
                for (auto use : uses)
                    if (use->isImm() && use->getVal() == 0)
                        flag = true;
                if (uses[0]->isParam())
                    flag = false;
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
        // auto ope = *(web->defs.begin());
        // cout << ope->getReg() << " ";
        web->disp = -func->AllocSpace(4);
        auto off = new MachineOperand(MachineOperand::IMM, web->disp);
        auto fp = new MachineOperand(MachineOperand::REG, 11);
        for (auto use : web->uses) {
            auto temp = new MachineOperand(*use);
            MachineOperand* operand = nullptr;
            if (web->disp > 255 || web->disp < -255) {
                operand = new MachineOperand(MachineOperand::VREG,
                                             SymbolTable::getLabel());
                auto inst1 =
                    new LoadMInstruction(use->getParent()->getParent(),
                                         LoadMInstruction::LDR, operand, off);
                use->getParent()->insertBefore(inst1);
            }
            if (operand) {
                if (!use->isFloat()) {
                    auto inst = new LoadMInstruction(
                        use->getParent()->getParent(), LoadMInstruction::LDR,
                        temp, fp, new MachineOperand(*operand));
                    use->getParent()->insertBefore(inst);

                } else {
                    auto reg = new MachineOperand(MachineOperand::VREG,
                                                  SymbolTable::getLabel());
                    MachineInstruction* inst = new BinaryMInstruction(
                        use->getParent()->getParent(), BinaryMInstruction::ADD,
                        reg, fp, new MachineOperand(*operand));
                    use->getParent()->insertBefore(inst);
                    inst = new LoadMInstruction(use->getParent()->getParent(),
                                                LoadMInstruction::VLDR, temp,
                                                new MachineOperand(*reg));
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
        for (auto def : web->defs) {
            if (!def->getParent())
                continue;
            auto temp = new MachineOperand(*def);
            MachineOperand* operand = nullptr;
            MachineInstruction *inst1 = nullptr, *inst = nullptr;
            if (web->disp > 255 || web->disp < -255) {
                operand = new MachineOperand(MachineOperand::VREG,
                                             SymbolTable::getLabel());
                inst1 =
                    new LoadMInstruction(def->getParent()->getParent(),
                                         LoadMInstruction::LDR, operand, off);
                def->getParent()->insertAfter(inst1);
            }
            if (operand) {
                if (!def->isFloat()) {
                    inst = new StoreMInstruction(
                        def->getParent()->getParent(), StoreMInstruction::STR,
                        temp, fp, new MachineOperand(*operand));
                } else {
                    auto reg = new MachineOperand(MachineOperand::VREG,
                                                  SymbolTable::getLabel());
                    MachineInstruction* tmp_inst = new BinaryMInstruction(
                        def->getParent()->getParent(), BinaryMInstruction::ADD,
                        reg, fp, new MachineOperand(*operand));

                    inst1->insertAfter(tmp_inst);
                    inst1 = tmp_inst;

                    inst = new StoreMInstruction(def->getParent()->getParent(),
                                                 StoreMInstruction::VSTR, temp,
                                                 new MachineOperand(*reg));
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
    // cout << endl;
}