#ifndef __LOOPUNROLL_H__
#define __LOOPUNROLL_H__

#include"BasicBlock.h"
#include"Instruction.h"
#include"SSAGraph.h"
#include"Operand.h"
#include<vector>
#include<stack>
#include<map>
#include<unordered_map>
#include<algorithm>
#include<iostream>


class loop{
    BasicBlock* cond;
    BasicBlock* body;
public:
    void setcond(BasicBlock* cond){this->cond=cond;};
    void setbody(BasicBlock* body){this->body=body;};
    BasicBlock* getcond(){return cond;};
    BasicBlock* getbody(){return body;};

};

class LoopUnroll{
    std::unordered_map<Function*, std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>> DomBBSet;
    std::vector<loop*> candidateLoops;
    int MAXUNROLLNUM=400;
    int UNROLLNUM=4;
public:
    void setDomBBset(std::unordered_map<Function*, std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>> DomBBSet){this->DomBBSet=DomBBSet;};
    void findLoop(std::vector<std::vector<BasicBlock*>> LoopList);
    bool isSubset(std::vector<BasicBlock*> son, std::vector<BasicBlock*> farther);
    std::vector<std::vector<BasicBlock*>> calculateInnerLoop(std::vector<std::vector<BasicBlock*>> LoopList);
    void Unroll();
    bool isRegionConst(Operand* i, Operand* c);
    Operand* getBeginOp(BasicBlock* bb,Operand* strideOp,std::stack<Instruction*>& Insstack);
    void specialCopyInstructions(BasicBlock* bb,int count,Operand* endOp,Operand* strideOp,bool ifall);
    void normalCopyInstructions(BasicBlock* condbb,BasicBlock* bodybb,Operand* beginOp,Operand* endOp,Operand* strideOp);
};

#endif