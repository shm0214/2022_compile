#ifndef __LOOPUNROLL_H__
#define __LOOPUNROLL_H__

#include"BasicBlock.h"
#include"Instruction.h"
#include<vector>
#include<algorithm>


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

    std::vector<loop*> candidateLoops;
public:
    void findLoop(std::vector<std::vector<BasicBlock*>> LoopList);
    bool isSubset(std::vector<BasicBlock*> son, std::vector<BasicBlock*> farther);
    std::vector<std::vector<BasicBlock*>> calculateInnerLoop(std::vector<std::vector<BasicBlock*>> LoopList);
    void Unroll();

};

#endif