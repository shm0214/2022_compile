#include "LoopUnroll.h"


bool LoopUnroll::isSubset(std::vector<BasicBlock*> son, std::vector<BasicBlock*> farther){
    for(auto sonBB:son){
        if(!count(farther.begin(),farther.end(),sonBB)){
            return false;
        }
    }
    return son.size()==farther.size();
}

std::vector<std::vector<BasicBlock*>> LoopUnroll::calculateInnerLoop(std::vector<std::vector<BasicBlock*>> LoopList){
    std::vector<std::vector<BasicBlock*>> InnerLoop;
    for(auto loopi:LoopList){
        bool isInner =true;
        for(auto loopj:LoopList){
            if(isSubset(loopi,loopj)){
                isInner=false;
                break;
            }
        }
        if(isInner){
            InnerLoop.push_back(loopi);
        }
    }
    return InnerLoop;
}


void LoopUnroll::findLoop(std::vector<std::vector<BasicBlock*>> InnerLoopList){
    for(auto InnerLoop:InnerLoopList){
        if(InnerLoop.size()!=2){
            continue;
        }
        BasicBlock* cond=nullptr;
        BasicBlock* body=nullptr;
        for(auto bb : InnerLoop){
            for(auto instr = bb->begin(); instr != bb->end()->getNext(); instr = instr->getNext()){
                if(instr->isCmp()){
                    cond = bb;
                    break;
                }
            }
            if(cond) break;
        }

        for(auto bb : InnerLoop){
            if(bb != cond){
                body = bb;
            }
        }

        if(body->getNumOfSucc() != 1){
            // std::cout<<"The succ of body only have one"<<std::endl;
            continue;
        }
        loop* l = new loop();
        l->setbody(body);
        l->setcond(cond);
        candidateLoops.push_back(l);
    }
}

void LoopUnroll::Unroll(){
    for(auto loop:candidateLoops){
        //包含call指令的不展开
        bool HasCallInBody = false;
        for(auto bodyinstr = loop->getbody()->begin(); bodyinstr != loop->getbody()->end()->getNext(); bodyinstr = bodyinstr->getNext()){
            if(bodyinstr->isCall()){
                HasCallInBody = true;
                break;
            }
        }
        if(HasCallInBody){
            // Exception("Candidate loop shall have no call in body");
            //std::cout<<"Candidate loop shall have no call in body"<<std::endl;
            continue;
        }
        /*
        * [Step 1] Calc begin, end, stride. They shall all be constant in Special LoopUnrolling.
        * [Step 2] Choose Normal LoopUnrolling or Special LoopUnrolling.
        * [Step 2.1] IF begin, end and stride are all constant, try Special LoopUnrolling.
        * [Step 2.2] IF stride is constant, while begin or end is not constant, try Normal LoopUnrolling.
        * 
        *                     --------
        *                     ↓      ↑
        * Special:  pred -> cond -> body  Exitbb      ==>     pred -> newbody -> Exitbb
        *                     ↓             ↑
        *                     ---------------
        * 
        * 
        * 
        *                     --------                                    ------------        -----------
        *                     ↓      ↑                                    ↓          ↑        ↓         ↑
        * Normal:  pred ->  cond -> body  Exitbb      ==>     pred -> maincond -> mainbody rescond -> resbody Exitbb
        *                     ↓             ↑                             ↓                  ↑   ↓              ↑
        *                     ---------------                             --------------------   ----------------
        * 
        */
        int begin = -1, end = -1, stride = -1;
        bool IsBeginCons, IsEndCons, IsStrideCons;
        IsBeginCons = IsEndCons = IsStrideCons = true;
        BasicBlock* cond = loop->getcond(), *body = loop->getbody();
        

    }
}
