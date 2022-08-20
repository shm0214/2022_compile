#include "PartialRedundancyElimination.h"
#include <sstream>

using namespace std;

// ref: Compilers:Principle,Techniques and Tools 9.5

void PartialRedundancyElimination::pass() {
    for (auto func : unit->getFuncs())
        pass(func);
}

void PartialRedundancyElimination::pass(MachineFunction* func) {
    ins.clear();
    expressions.clear();
    expr2ins.clear();
    ope2expr.clear();
    use.clear();
    kill.clear();
    anticipatedIn.clear();
    anticipatedOut.clear();
    availableIn.clear();
    availableOut.clear();
    earliest.clear();
    postponableIn.clear();
    postponableOut.clear();
    latest.clear();
    usedIn.clear();
    usedOut.clear();
    insertEmptyBlock(func);
    computeExpression(func);
    computeUseKill(func);
    computeAnticipated(func);
    computeAvailable(func);
    computeEarliest(func);
    computePostponable(func);
    computeLatest(func);
    computeUsed(func);
    modifyCode(func);
    removeExit(func);
}

void PartialRedundancyElimination::insertEmptyBlock(MachineFunction* func) {
    // pred, succ
    map<MachineBlock*, MachineBlock*> edges;
    for (auto block : func->getBlocks()) {
        auto succs = block->getSuccs();
        for (auto succ : succs)
            if (succ->getPreds().size() >= 2)
                edges[block] = succ;
    }
    for (auto it : edges) {
        auto pred = it.first;
        auto succ = it.second;
        auto no = SymbolTable::getLabel();
        auto newBlock = new MachineBlock(func, no);
        pred->removeSucc(succ);
        succ->removePred(pred);
        pred->addSucc(newBlock);
        succ->addPred(newBlock);
        newBlock->addPred(pred);
        newBlock->addSucc(succ);
        stringstream s;
        s << ".L" << succ->getNo();
        auto dst = new MachineOperand(s.str());
        auto branch =
            new BranchMInstruction(newBlock, BranchMInstruction::B, dst);
        newBlock->InsertInst(branch);
        // straighten 需要利用顺序消除跳转 因此维护插入顺序
        func->InsertAfter(pred, newBlock);
        // change branch dst in pred
        auto insts = pred->getInsts();
        bool flag = false;
        for (auto it = insts.rbegin(); it != insts.rend(); it++) {
            if (!(*it)->isB())
                break;
            auto use = (*it)->getUse()[0];
            auto label = use->getLabel().substr(2);
            auto next = func->getBlock(stoi(label));
            if (next == succ) {
                flag = true;
                s.str("");
                s << ".L" << no;
                auto oper = new MachineOperand(s.str());
                (*it)->replaceUse(use, oper);
            }
        }
        assert(flag);
    }
    // insert exit
    exit = new MachineBlock(func, -1);
    func->InsertBlock(exit);
    for (auto block : func->getBlocks()) {
        if (block == exit)
            continue;
        auto in = *(block->getInsts().rbegin());
        if (!in->isBX())
            continue;
        block->addSucc(exit);
        exit->addPred(block);
    }
}

void PartialRedundancyElimination::computeExpression(MachineFunction* func) {
    for (auto block : func->getBlocks())
        for (auto in : block->getInsts()) {
            auto type = in->getType();
            // 感觉load还是不太行 全局变量可以做局部化 因为store有可能出错
            // 如果不好做的话这里单独判断下 只对全局地址取吧
            if (type != MachineInstruction::BINARY &&
                type != MachineInstruction::LOAD)
                // if (type != MachineInstruction::BINARY)
                continue;
            ins.push_back(in);
            auto uses = in->getUse();
            if (in->isLoad())
                if (uses.size() != 1 || !uses[0]->isLabel())
                    continue;
            Expression expr;
            get<0>(expr) = type;
            get<1>(expr) = in->getOp();
            for (auto use : uses)
                get<2>(expr).push_back(*use);
            expr2ins[expr].push_back(in);
            expressions.insert(expr);
            for (auto use : uses)
                ope2expr[*use].insert(expr);
        }
}

void PartialRedundancyElimination::computeUseKill(MachineFunction* func) {
    for (auto block : func->getBlocks())
        for (auto in : block->getInsts()) {
            auto type = in->getType();
            if (type == MachineInstruction::BINARY ||
                type == MachineInstruction::LOAD) {
                // if (type == MachineInstruction::BINARY) {
                auto uses = in->getUse();
                // if (in->isLoad())
                //     if (uses.size() != 1 || !uses[0]->isLabel())
                //         continue;
                Expression expr;
                get<0>(expr) = type;
                get<1>(expr) = in->getOp();
                for (auto use : uses)
                    get<2>(expr).push_back(*use);
                if (!kill[block].count(expr))
                    use[block].insert(expr);
            }
            for (auto def : in->getDef())
                kill[block].insert(ope2expr[*def].begin(),
                                   ope2expr[*def].end());
        }
}

void PartialRedundancyElimination::computeAnticipated(MachineFunction* func) {
    for (auto block : func->getBlocks())
        anticipatedIn[block] = expressions;
    anticipatedIn[exit].clear();
    bool change = true;
    while (change) {
        change = false;
        for (auto block : func->getBlocks()) {
            if (block == exit)
                continue;
            anticipatedOut[block] = expressions;
            auto old = anticipatedIn[block];
            for (auto succ : block->getSuccs()) {
                set<Expression> temp;
                set_intersection(
                    anticipatedIn[succ].begin(), anticipatedIn[succ].end(),
                    anticipatedOut[block].begin(), anticipatedOut[block].end(),
                    inserter(temp, temp.end()));
                anticipatedOut[block] = temp;
            }
            anticipatedIn[block] = use[block];
            set_difference(
                anticipatedOut[block].begin(), anticipatedOut[block].end(),
                kill[block].begin(), kill[block].end(),
                inserter(anticipatedIn[block], anticipatedIn[block].end()));
            if (anticipatedIn[block] != old)
                change = true;
        }
    }
}

void PartialRedundancyElimination::computeAvailable(MachineFunction* func) {
    for (auto block : func->getBlocks())
        availableOut[block] = expressions;
    availableIn[func->getEntry()].clear();
    bool change = true;
    while (change) {
        change = false;
        for (auto block : func->getBlocks()) {
            if (block == func->getEntry())
                continue;
            availableIn[block] = expressions;
            auto old = availableOut[block];
            for (auto pred : block->getPreds()) {
                set<Expression> temp;
                set_intersection(
                    availableOut[pred].begin(), availableOut[pred].end(),
                    availableIn[block].begin(), availableIn[block].end(),
                    inserter(temp, temp.end()));
                availableIn[block] = temp;
            }
            availableOut[block] = anticipatedIn[block];
            availableOut[block].insert(availableIn[block].begin(),
                                       availableIn[block].end());
            for (auto k : kill[block])
                if (availableOut[block].count(k))
                    availableOut[block].erase(k);
            if (availableOut[block] != old)
                change = true;
        }
    }
}

void PartialRedundancyElimination::computeEarliest(MachineFunction* func) {
    for (auto block : func->getBlocks())
        set_difference(anticipatedIn[block].begin(), anticipatedIn[block].end(),
                       availableIn[block].begin(), availableIn[block].end(),
                       inserter(earliest[block], earliest[block].end()));
}

void PartialRedundancyElimination::computePostponable(MachineFunction* func) {
    for (auto block : func->getBlocks())
        postponableOut[block] = expressions;
    postponableIn[func->getEntry()].clear();
    bool change = true;
    while (change) {
        change = false;
        for (auto block : func->getBlocks()) {
            if (block == func->getEntry())
                continue;
            postponableIn[block] = expressions;
            auto old = postponableOut[block];
            for (auto pred : block->getPreds()) {
                set<Expression> temp;
                set_intersection(
                    postponableOut[pred].begin(), postponableOut[pred].end(),
                    postponableIn[block].begin(), postponableIn[block].end(),
                    inserter(temp, temp.end()));
                postponableIn[block] = temp;
            }
            postponableOut[block] = earliest[block];
            postponableOut[block].insert(postponableIn[block].begin(),
                                         postponableIn[block].end());
            for (auto k : use[block])
                if (postponableOut[block].count(k))
                    postponableOut[block].erase(k);
            if (postponableOut[block] != old)
                change = true;
        }
    }
}

void PartialRedundancyElimination::computeLatest(MachineFunction* func) {
    for (auto block : func->getBlocks()) {
        auto temp = earliest[block];
        temp.insert(postponableIn[block].begin(), postponableIn[block].end());
        auto temp1 = use[block];
        auto temp2 = expressions;
        for (auto succ : block->getSuccs()) {
            auto temp3 = earliest[succ];
            temp3.insert(postponableIn[succ].begin(),
                         postponableIn[succ].end());
            set<Expression> temp4;
            set_intersection(temp2.begin(), temp2.end(), temp3.begin(),
                             temp3.end(), inserter(temp4, temp4.end()));
            temp2 = temp4;
        }
        set_difference(expressions.begin(), expressions.end(), temp2.begin(),
                       temp2.end(), inserter(temp1, temp1.end()));
        set_intersection(temp.begin(), temp.end(), temp1.begin(), temp1.end(),
                         inserter(latest[block], latest[block].end()));
    }
}

void PartialRedundancyElimination::computeUsed(MachineFunction* func) {
    for (auto block : func->getBlocks())
        usedIn[block].clear();
    bool change = true;
    while (change) {
        change = false;
        for (auto block : func->getBlocks()) {
            if (block == exit)
                continue;
            usedOut[block].clear();
            auto old = usedIn[block];
            for (auto succ : block->getSuccs())
                usedOut[block].insert(usedIn[succ].begin(), usedIn[succ].end());
            auto temp = use[block];
            temp.insert(usedOut[block].begin(), usedOut[block].end());
            set_difference(temp.begin(), temp.end(), latest[block].begin(),
                           latest[block].end(),
                           inserter(usedIn[block], usedIn[block].end()));
            if (usedIn[block] != old)
                change = true;
        }
    }
}

void PartialRedundancyElimination::modifyCode(MachineFunction* func) {
    map<Expression, MachineOperand*> expr2ope;
    for (auto block : func->getBlocks()) {
        set<Expression> exprs;
        set_intersection(latest[block].begin(), latest[block].end(),
                         usedOut[block].begin(), usedOut[block].end(),
                         inserter(exprs, exprs.end()));
        for (auto expr : exprs) {
            if (!expr2ope.count(expr)) {
                bool fpu = false;
                for (auto o : get<2>(expr))
                    if (o.isFloat()) {
                        fpu = true;
                        break;
                    }
                auto ope = new MachineOperand(MachineOperand::VREG,
                                              SymbolTable::getLabel(), fpu);
                expr2ope[expr] = ope;
            }
            auto dst = new MachineOperand(*expr2ope[expr]);
            int type = get<0>(expr);
            MachineInstruction* newIn;
            auto uses = get<2>(expr);
            int op = get<1>(expr);
            if (type == MachineInstruction::BINARY) {
                auto src1 = new MachineOperand(uses[0]);
                auto src2 = new MachineOperand(uses[1]);
                newIn = new BinaryMInstruction(block, op, dst, src1, src2);
            } else if (type == MachineInstruction::LOAD) {
                auto src1 = new MachineOperand(uses[0]);
                MachineOperand* src2 = nullptr;
                if (uses.size() > 1)
                    src2 = new MachineOperand(uses[1]);
                newIn = new LoadMInstruction(block, op, dst, src1, src2);
            }
            block->insertFront(newIn);
        }
    }
    for (auto in : ins) {
        auto block = in->getParent();
        auto uses = in->getUse();
        Expression expr;
        get<0>(expr) = in->getType();
        get<1>(expr) = in->getOp();
        for (auto use : uses)
            get<2>(expr).push_back(*use);
        if (!use[block].count(expr))
            continue;
        if (latest[block].count(expr) && !usedOut[block].count(expr))
            continue;
        auto dst = new MachineOperand(*(in->getDef()[0]));
        if (!expr2ope.count(expr))
            continue;
        auto src = new MachineOperand(*(expr2ope[expr]));
        MachineInstruction* newIn;
        if (src->isFloat() || dst->isFloat())
            newIn = new MovMInstruction(block, MovMInstruction::VMOV, dst, src);
        else
            newIn = new MovMInstruction(block, MovMInstruction::MOV, dst, src);
        block->insertBefore(newIn, in);
        block->remove(in);
    }
}

void PartialRedundancyElimination::removeExit(MachineFunction* func) {
    for (auto pred : exit->getPreds())
        pred->removeSucc(exit);
    func->removeBlock(exit);
    // 应该问题不大
    delete exit;
}