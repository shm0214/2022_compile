#ifndef __CONTROL_FLOW_ANALYSIS_H__
#define __CONTROL_FLOW_ANALYSIS_H__

#include "Unit.h"

class ControlFlowAnalysis {
   private:
    enum { TREE, FORWARD, BACK, CROSS };
    int i, j;
    std::map<std::pair<MachineBlock*, MachineBlock*>, int> edges;
    std::set<MachineBlock*> visited;
    std::map<MachineBlock*, int> preOrder;
    std::map<MachineBlock*, int> postOrder;
    std::map<MachineBlock*, int> loopDepth;

   public:
    void dfs(MachineBlock* block);
    void pass(MachineFunction* func);
    std::set<MachineBlock*> naturalLoop(MachineBlock* b1, MachineBlock* b2);
    void computeLoopDepth();
    int getLoopDepth(MachineBlock* block) { return loopDepth[block]; }
};
#endif