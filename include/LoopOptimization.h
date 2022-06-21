
#include <vector>
#include <unordered_map>
#include "Unit.h"
#include "Function.h"
#include "BasicBlock.h"


//针对函数进行优化，对于函数取entry
class LoopOptimization{
    Function* func;
    //针对每一个bb 求必经节点
    //key value
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>DomBBSet;
    std::vector<std::vector<BasicBlock*>> LoopList;
public:
    //代码外提
    LoopOptimization(Function* func):func(func){};
    void initializeDomBBSet();
    bool ifDomBBSetChange(std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> lastSet);
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>& getDomBBSet(){return DomBBSet;};
    std::vector<BasicBlock*> getIntersectBBList(std::vector<BasicBlock*> List1,std::vector<BasicBlock*>List2);
    void calculateFinalDomBBSet();
    void calculateLoopList();
    void CodePullUp();


    

};