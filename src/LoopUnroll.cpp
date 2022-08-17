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
        int opcode;
        bool IsBeginCons, IsEndCons, IsStrideCons;
        IsBeginCons = IsEndCons = IsStrideCons = true;
        BasicBlock* cond = loop->getcond(), *body = loop->getbody();
        CmpInstruction* cmp;
        //可以计算出基本归纳变量表存储信息 利用信息


        //cal begin end stride
        //都是const
        //获取所有的归纳变量 如果所有的归纳变量对应的初始
        //直接利用cmp确定即可
        //由SSA图再找stride

        if(IsBeginCons&&IsEndCons&&IsStrideCons){
            //完全展开
            //计算多少次
            if(opcode==SSAGraphNode::ADD){
                if(cmp->getOpcode()==CmpInstruction::G){
                    int count=-1;
                    for(int i=begin;i<end;i=i+stride){
                        count++;
                    }
                    //指令copy count 份
                    //body中的跳转指令不copy

                }
                else if(cmp->getOpcode()==CmpInstruction::GE){
                    
                }
                else if(cmp->getOpcode()==CmpInstruction::L){
                    
                }
                else if(cmp->getOpcode()==CmpInstruction::LE){
                    
                }
            }
            else if(opcode==SSAGraphNode::SUB){

            }
            else if(opcode==SSAGraphNode::MUL){

            }
            else if(opcode==SSAGraphNode::DIV){

            }
            else {
                std::cout<<"stride calculate not add sub mul div"<<std::endl;
                continue;
            }
        }
        else if(IsStrideCons){
            //展开四次
            //copy四次


            //构建rescond resbody
            //rescond包含 最后算出来的变量值 然后新建一条cmp指令 最后算出来的变量值与end作比较，重构一个循环即可
            

        }


    }
}
