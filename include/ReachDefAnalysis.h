#ifndef __REACH_DEF_ANALYSIS_H__
#define __REACH_DEF_ANALYSIS_H__

#include "Unit.h"

class ReachDefAnalysis {
   private:
    std::map<MachineOperand, std::set<MachineOperand*>> all_defs;
    std::map<MachineBlock*, std::set<MachineOperand*>> gen, kill;
    void computeDefPos(MachineFunction*);
    void computeGenKill(MachineFunction*);
    void iterate(MachineFunction*);

   public:
    void pass(MachineUnit* unit);
    void pass(MachineFunction* func);
    std::map<MachineOperand, std::set<MachineOperand*>>& getAllDefs() {
        return all_defs;
    };
};

#endif