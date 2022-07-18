#ifndef __LOOP_OPTIMIZATION_H__
#define __LOOP_OPTIMIZATION_H__
#include <vector>
#include <unordered_map>
#include "Unit.h"
#include "Function.h"
#include "BasicBlock.h"


//针对函数进行优化，对于函数取entry
class LoopOptimization{
    Unit *unit;
    //针对每一个bb 求必经节点
    //key value
    std::unordered_map<Function*, std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>> DomBBSet;
public:
    //代码外提
    LoopOptimization(Unit* unit):unit(unit){};
    void initializeDomBBSet(Function* func);
    bool ifDomBBSetChange(std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>& lastSet,Function* func);
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>& getDomBBSet(Function* func){return DomBBSet[func];};
    std::vector<BasicBlock*> getIntersectBBList(std::vector<BasicBlock*>& List1,std::vector<BasicBlock*>& List2);
    void calculateFinalDomBBSet(Function* func);
    std::vector<std::pair<BasicBlock*,BasicBlock*>> getBackEdges(Function* func);
    std::vector<std::vector<std::pair<BasicBlock*,BasicBlock*>>> mergeEdge(std::vector<std::pair<BasicBlock*,BasicBlock*>>& BackEdges);
    std::vector<std::vector<BasicBlock*>> calculateLoopList(Function* func,std::vector<std::pair<BasicBlock*,BasicBlock*>>& BackEdges);
    void delteglobalsubexpr();
    bool defInstructionInLoop(Operand * op,std::vector<BasicBlock*>Loop);
    bool OperandIsLoopConst(Operand * op,std::vector<Instruction*> LoopConstInstructions);
    std::vector<Instruction*> calculateLoopConstant(std::vector<BasicBlock*>Loop);
    std::vector<BasicBlock*> calculateOutBlock(std::vector<BasicBlock*>& Loop);
    void changePhiInstruction(std::vector<BasicBlock*>& Loop,BasicBlock* preBlock,std::vector<BasicBlock*> oldBlocks);
    void CodePullUp(Function* func,std::vector<std::vector<BasicBlock*>>& LoopList,std::vector<std::pair<BasicBlock*,BasicBlock*>>& BackEdges);
    void printDomBB(Function * func);
    void printLoop(std::vector<std::vector<BasicBlock*>>& LoopList);
    void printLoopConst(std::vector<Instruction*> LoopConstInstructions);


    void pass();

};

#endif