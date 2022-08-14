#ifndef __PARTIAL_REDUNDANCY_ELIMINATION_H__
#define __PARTIAL_REDUNDANCY_ELIMINATION_H__

#include "MachineCode.h"
#include <iostream>

class PartialRedundancyElimination {
    MachineUnit* unit;
    MachineBlock* exit;
    // 还得保留顺序 那就没办法消a+b与b+a了
    using Expression = std::tuple<int, int, std::vector<MachineOperand>>;
    std::vector<MachineInstruction*> ins;
    std::set<Expression> expressions;
    std::map<Expression, std::vector<MachineInstruction*>> expr2ins;
    std::map<MachineOperand, std::set<Expression>> ope2expr;
    std::map<MachineBlock*, std::set<Expression>> use;
    std::map<MachineBlock*, std::set<Expression>> kill;
    std::map<MachineBlock*, std::set<Expression>> anticipatedIn;
    std::map<MachineBlock*, std::set<Expression>> anticipatedOut;
    std::map<MachineBlock*, std::set<Expression>> availableIn;
    std::map<MachineBlock*, std::set<Expression>> availableOut;
    std::map<MachineBlock*, std::set<Expression>> earliest;
    std::map<MachineBlock*, std::set<Expression>> postponableIn;
    std::map<MachineBlock*, std::set<Expression>> postponableOut;
    std::map<MachineBlock*, std::set<Expression>> latest;
    std::map<MachineBlock*, std::set<Expression>> usedIn;
    std::map<MachineBlock*, std::set<Expression>> usedOut;

   public:
    PartialRedundancyElimination(MachineUnit* unit) : unit(unit) {};
    void pass();
    void pass(MachineFunction* func);
    void insertEmptyBlock(MachineFunction* func);
    void computeExpression(MachineFunction* func);
    void computeUseKill(MachineFunction* func);
    void computeAnticipated(MachineFunction* func);
    void computeAvailable(MachineFunction* func);
    void computeEarliest(MachineFunction* func);
    void computePostponable(MachineFunction* func);
    void computeLatest(MachineFunction* func);
    void computeUsed(MachineFunction* func);
    void modifyCode(MachineFunction* func);
    void removeExit(MachineFunction* func);
};

#endif