#ifndef __LOOP_OPTIMIZATION_H__
#define __LOOP_OPTIMIZATION_H__
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <assert.h>
#include "Unit.h"
#include "Function.h"
#include "BasicBlock.h"
#include "SSAGraph.h"
#include "Operand.h"
#include "IV_record.h"
#include "reduceOperate.h"
#include "LoopUnroll.h"

#define min(a,b)    (((a) < (b)) ? (a) : (b))

//针对函数进行优化，对于函数取entry
class LoopOptimization{
    Unit *unit;
    //针对每一个bb 求必经节点
    //key value
    std::unordered_map<Function*, std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>> DomBBSet;
    std::map<Function*, std::map<std::vector<BasicBlock*>,std::set<Operand*>>> LoopConst;
    std::map<Function*, std::vector<reduceOperate*>> reduceTable;
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
    std::vector<Instruction*> calculateLoopConstant(std::vector<BasicBlock*>Loop,Function* func);
    std::vector<BasicBlock*> calculateOutBlock(std::vector<BasicBlock*>& Loop);
    void changePhiInstruction(std::vector<BasicBlock*>& Loop,BasicBlock* preBlock,std::vector<BasicBlock*> oldBlocks);
    void CodePullUp(Function* func,std::vector<std::vector<BasicBlock*>>& LoopList,std::vector<std::pair<BasicBlock*,BasicBlock*>>& BackEdges);
    void printDomBB(Function * func);
    void printLoop(std::vector<std::vector<BasicBlock*>>& LoopList);
    void printLoopConst(std::vector<Instruction*> LoopConstInstructions);
    //强度削弱
    // bool ifFindDependLoop(std::vector<BasicBlock*> Loop,Operand* backOperand, Operand* beginOperand, std::set<Operand*> LoopConstList,std::vector<Instruction*>&InstList);
    // std::map<std::vector<BasicBlock*>,std::vector<IV_records*>> findInductionVariable(Function * func, std::vector<std::vector<BasicBlock*>>& LoopList);
    // std::set<Operand*>& getLoopConstList(Function* func,std::vector<BasicBlock*> Loop){return LoopConst[func][Loop];};
    // bool ifInIVList(Operand* op,std::vector<IV_records*> IVs,IV_records* biv);
    // bool ifIV(BinaryInstruction* bin,std::set<Operand*> &LoopConstList,std::vector<IV_records*> IVs,IV_records* iv);
    Operand* replaceCmpIns(Instruction* cmpIns,reduceOperate* reduceOp,Operand* useOp);
    void LFTR(std::vector<BasicBlock*>& loop,std::vector<Instruction*> CmpInsList,Function* func);
    std::vector<SSAGraphNode*> getLoopSSAGraph(std::vector<BasicBlock*>Loop,std::vector<SSAGraphNode*>Graph,std::vector<SSAGraphNode*>& NotCandidateList,std::vector<Instruction*>&CmpInsList);
    void OSR(std::vector<SSAGraphNode*> graph,Function* func,std::vector<std::vector<BasicBlock*>> LoopList);
    void resetIVOp(std::vector<BasicBlock*>& loop);
    void DFS(SSAGraphNode* n,std::vector<SSAGraphNode*>& NodeStack,std::map<IV_record*,Operand*>&NodeHashTable,Function* func,std::vector<SSAGraphNode*> NotCandidateList);
    void Process(std::vector<SSAGraphNode*>& SCC,std::map<IV_record*,Operand*>&NodeHashTable,Function* func,std::vector<SSAGraphNode*> NotCandidateList);
    void ClassifyIV(std::vector<SSAGraphNode*>& SCC,std::map<IV_record*,Operand*>&NodeHashTable,Function* func,std::vector<SSAGraphNode*> NotCandidateList);
    bool isCandidate(SSAGraphNode* n,SSAGraphNode* & iv,SSAGraphNode* & rc,std::vector<SSAGraphNode*> NotCandidateList);
    bool isLocalVar(SSAGraphNode* i,SSAGraphNode* c);
    bool isValidUpdate(SSAGraphNode* n);
    SSAGraphNode* getRPONode(std::vector<SSAGraphNode*>& SCC);
    void search(SSAGraphNode* n,std::map<SSAGraphNode*,bool>& visit,std::map<SSAGraphNode*,int>& dfn);
    void replace(SSAGraphNode* n,SSAGraphNode* iv,SSAGraphNode* rc,std::map<IV_record*,Operand*>&NodeHashTable,Function* func);
    Operand* reduce(IV_record* iv_record,std::map<IV_record*,Operand*>&NodeHashTable,Function* func);
    Operand* ifInHashTable(std::map<IV_record*,Operand*>&NodeHashTable,IV_record* iv_record);
    void rewrite(Operand* _new,Operand* _old,Function* func);
    Operand* apply(IV_record* iv,std::map<IV_record*,Operand*>&NodeHashTable,Function* func);
    void printSCC(std::vector<SSAGraphNode*> SCC);
    void dealwithNoPreBB(Function* func);

    void pass();
    void pass1();

};

#endif