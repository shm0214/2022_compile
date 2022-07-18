#include "LoopOptimization.h"
#include <algorithm>

//after ssa
bool LoopOptimization :: ifDomBBSetChange(std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>& lastSet,Function* func){
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> DomBB_map=getDomBBSet(func);
    for(auto block: func->getBlockList()){
        if(lastSet[block]!=DomBB_map[block]){
            return true;
        }
    }
    return false;
}


void LoopOptimization :: initializeDomBBSet(Function* func){
    if(DomBBSet.find(func)==DomBBSet.end()){
        std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>domBBMap;
        DomBBSet[func]=domBBMap;
    }
    for (auto block : func->getBlockList()) {
        if(DomBBSet[func].find(block)==DomBBSet[func].end()){
            if(block==func->getEntry()){
                std::vector<BasicBlock*> dom_list;
                dom_list.push_back(func->getEntry());
                DomBBSet[func][block]=dom_list;
            }
            else{
                std::vector<BasicBlock*> dom_list(func->getBlockList());
                DomBBSet[func][block]=dom_list;
            }
        }
    }
    return;
}

std::vector<BasicBlock*> LoopOptimization::getIntersectBBList(std::vector<BasicBlock*>& List1,std::vector<BasicBlock*>& List2){
    std::vector<BasicBlock*> finalList;
    for (auto block: List1){
        if(count(List2.begin(), List2.end(), block)){
            finalList.push_back(block);
        }
    }
    return finalList;
}

void LoopOptimization::calculateFinalDomBBSet(Function* func){
    initializeDomBBSet(func);
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> lastSet(getDomBBSet(func));
    while(true){
        std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>DomSet(getDomBBSet(func));
        for(auto block: func->getBlockList()){
            std::vector<BasicBlock*> finalDomList;
            bool ifFirst=true;
            for (auto fartherBlock=block->pred_begin();fartherBlock!=block->pred_end();fartherBlock++){
                if(ifFirst){
                    finalDomList=DomSet[*fartherBlock];
                    ifFirst=false;
                    continue;
                }
                finalDomList=getIntersectBBList(DomSet[*fartherBlock],finalDomList);
            }
            if(!count(finalDomList.begin(),finalDomList.end(),block)){
                finalDomList.push_back(block);
            }
            DomSet[block]=finalDomList;
        }
        DomBBSet[func]=DomSet;
        if(!ifDomBBSetChange(lastSet,func)){
            break;
        }
        else{
            lastSet=getDomBBSet(func);
        }
    }
    return;
}

std::vector<std::vector<std::pair<BasicBlock*,BasicBlock*>>> LoopOptimization::mergeEdge(std::vector<std::pair<BasicBlock*,BasicBlock*>>& BackEdges){
    std::vector<std::vector<std::pair<BasicBlock*,BasicBlock*>>> edgeGroups;
    for(auto edge:BackEdges){
        if(edgeGroups.size()==0){
            std::vector<std::pair<BasicBlock*,BasicBlock*>> tempgroup;
            tempgroup.push_back(edge);
            edgeGroups.push_back(tempgroup);
        }
        else{
            bool find_group=false;
            for(auto group:edgeGroups){
                if(group[0].second==edge.second){
                    group.push_back(edge);
                    find_group=true;
                    break;
                }
            }
            if(!find_group){
                std::vector<std::pair<BasicBlock*,BasicBlock*>> tempgroup;
                tempgroup.push_back(edge);
                edgeGroups.push_back(tempgroup);
            }
        }
    }
    return edgeGroups;
}

std::vector<std::pair<BasicBlock*,BasicBlock*>> LoopOptimization::getBackEdges(Function* func){
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> DomSet=getDomBBSet(func);
    std::vector<std::pair<BasicBlock*,BasicBlock*>> BackEdges;
    //search for backedges
    // std::cout<<"backedges:"<<std::endl;
    for (auto block:func->getBlockList()){
        for (auto domBB:DomSet[block]){
            for(auto succ_block=block->succ_begin();succ_block<block->succ_end();succ_block++){
                if(*succ_block==domBB){
                    std::pair<BasicBlock*,BasicBlock*> edge(block,domBB);
                    BackEdges.push_back(edge);
                    // std::cout<<block->getNo()<<" "<<domBB->getNo()<<std::endl;
                }
            }
        }
    }
    return BackEdges;
}

std::vector<std::vector<BasicBlock*>> LoopOptimization::calculateLoopList(Function* func,std::vector<std::pair<BasicBlock*,BasicBlock*>>& BackEdges){
    std::vector<std::vector<BasicBlock*>> LoopList;
    std::vector<std::vector<std::pair<BasicBlock*,BasicBlock*>>> edgeGroups=mergeEdge(BackEdges);
    //search for natural loop

    for(auto group:edgeGroups){
        std::vector<BasicBlock*> Loop;
        BasicBlock* headBlock=group[0].second;
        Loop.push_back(headBlock);
        for (auto edge:group){
            std::vector<BasicBlock*> Lastadd;      
            if(!count(Loop.begin(),Loop.end(),edge.first)){
                Loop.push_back(edge.first);
                Lastadd.push_back(edge.first);
            }
            else{
                continue;
            }
            
            while(true){
                std::vector<BasicBlock*> tempadd;
                bool ifAddNewBB=false;
                for(auto block:Lastadd){
                    for(auto fartherBB=block->pred_begin();fartherBB!=block->pred_end();fartherBB++){
                        if(!count(Loop.begin(),Loop.end(),*fartherBB)){
                            ifAddNewBB=true;
                            Loop.push_back(*fartherBB);
                            tempadd.push_back(*fartherBB);
                        }
                    }         
                }
                if(!ifAddNewBB)
                    break;
                Lastadd.assign(tempadd.begin(),tempadd.end());
            }
        }
        LoopList.push_back(Loop);
    }
    return LoopList;
}

bool LoopOptimization::defInstructionInLoop(Operand * op,std::vector<BasicBlock*>Loop){
    for(auto block:Loop){
        Instruction* i = block->begin();
        Instruction* last = block->end();
        while (i != last) {
            if(i->getDef()==op){
                return true;
            }
            i = i->getNext();
        }
    }
    return false;
}

bool LoopOptimization::OperandIsLoopConst(Operand * op,std::vector<Instruction*> LoopConstInstructions){
    //只有一个定值能到达,做了ssa，则只有一个def
    for(auto ins:LoopConstInstructions){
        if(ins->getDef()==op){
            return true;
        }
    }
    return false;
}
//bin & alloc 
std::vector<Instruction*> LoopOptimization::calculateLoopConstant(std::vector<BasicBlock*> Loop){
    std::vector<Instruction*> LoopConstInstructions;
    for(auto block:Loop){
        while(true){
            Instruction* ins = block->begin();
            Instruction* last = block->end();
            bool ifAddNew=false;
            while (ins != last) {
                if(!count(LoopConstInstructions.begin(),LoopConstInstructions.end(),ins)){
                    if(ins->isBin()){
                        std::vector<Operand*> useOperands=ins->getUse();
                        int constant_count=0;
                        //补充浮点数。。。。
                        //数组？
                        //指针运算？
                        //先做int吧
                        for(auto operand:useOperands){
                            Type * operand_type=operand->getEntry()->getType();
                            if(operand_type->isInt()/*||operand_type->isFloat()*/){
                                IntType* operand_inttype=(IntType*)operand_type;
                                if(operand_inttype->isConst()){
                                    constant_count++;
                                }
                                //定值点不在循环里,所有给它赋值的语句都在LOOP外面
                                else if(!defInstructionInLoop(operand,Loop)){
                                    constant_count++;
                                }
                                //只有一个到达定值
                                else if(OperandIsLoopConst(operand,LoopConstInstructions)){
                                    constant_count++;
                                }
                            }
                        }
                        if(constant_count==2){
                            // ins->output();
                            LoopConstInstructions.push_back(ins);
                            ifAddNew=true;
                        }
                    }
                }
                ins=ins->getNext();
            }
            if(ifAddNew){
                continue;
            }
            else{
                break;
            }
        }

    }
    return LoopConstInstructions;
}

std::vector<BasicBlock*> LoopOptimization::calculateOutBlock(std::vector<BasicBlock*>& Loop){
    std::vector<BasicBlock*> outBlock;
    for(auto block:Loop){
        //查找后继节点看是否在循环里
        for(auto succBB=block->succ_begin();succBB!=block->succ_end();succBB++){
            if(!count(Loop.begin(),Loop.end(),*succBB)){
                outBlock.push_back(block);
            }
        }
    }
    return outBlock;
}
//phi指令的块不在循环里，改成对应的preblock
void LoopOptimization::changePhiInstruction(std::vector<BasicBlock*>& Loop,BasicBlock* preBlock, std::vector<BasicBlock*> oldBlocks){
    for(auto block:Loop){
        Instruction* i = block->begin();
        Instruction* last = block->end();
        while (i != last) {
            //是phi指令
            if(i->isPhi()){
                PhiInstruction* pi=(PhiInstruction*)i;
                //判断phi指令的src块是否在循环里

                for(auto oldBlock:oldBlocks){
                    
                    if(pi->findSrc(oldBlock)){
                        Operand* op=pi->getSrc(oldBlock);
                        pi->removeSrc(oldBlock);
                        pi->addSrc(preBlock,op);
                    }
                }
            }
            i = i->getNext();
        }
    }
}

void LoopOptimization::CodePullUp(Function* func,std::vector<std::vector<BasicBlock*>>& LoopList, std::vector<std::pair<BasicBlock*,BasicBlock*>>& BackEdges){
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> domBBSet=getDomBBSet(func);
    // std::vector<std::pair<BasicBlock*,BasicBlock*>> backEdges=Func_backedges[func];
    for(auto Loop:LoopList){
        // std::cout<<"Loop:"<<std::endl;
        std::vector<BasicBlock*> outBlock=calculateOutBlock(Loop);
        std::vector<Instruction*> LoopConstInstructions=calculateLoopConstant(Loop);
        // printLoopConst(LoopConstInstructions);
        // std::cout<<LoopConstInstructions.size()<<std::endl;
        BasicBlock * headBlock=Loop[0];
        BasicBlock * predBB=new BasicBlock(func);
        bool ifAddFirst=true;
        for(auto ins:LoopConstInstructions){
            BasicBlock* fartherBB=ins->getParent();
            //farther BB 为所有出口节点的支配节点
            bool if_DomBBAll=true;
            for(auto block:outBlock){
                std::vector<BasicBlock*> domBBList=domBBSet[block];
                if(!count(domBBList.begin(),domBBList.end(),fartherBB)){
                    if_DomBBAll=false;
                    break;
                }
            }
            // 可以外提
            
            if(if_DomBBAll){
                //设置predBB的跳转，以及BasciBlock中的跳转指令的目的块
                if(ifAddFirst){
                    ifAddFirst=false;
                    //查headblock前驱最后一条指令然后改掉，指向predBB
                    std::vector<BasicBlock*> pre_block_delete;
                    for(auto block=headBlock->pred_begin();block!=headBlock->pred_end();block++){
                        //只用改不是回边的块的前驱后继
                        std::pair<BasicBlock*,BasicBlock*> edge(*block,headBlock);
                        if(count(BackEdges.begin(), BackEdges.end(), edge)){
                            continue;
                        }
                        pre_block_delete.push_back(*block);
                        Instruction* lastins=(*block)->rbegin();
                        if(lastins->isCond()){
                            CondBrInstruction* last =(CondBrInstruction*)lastins;
                            if(last->getTrueBranch()==headBlock){
                                
                                last->setTrueBranch(predBB);
                            }
                            else if(last->getFalseBranch()==headBlock){
                                
                                last->setFalseBranch(predBB);
                            }
                        }
                        else if(lastins->isUncond()){
                            UncondBrInstruction* last=(UncondBrInstruction*)lastins;
                            last->setBranch(predBB);
                        }
                        (*block)->removeSucc(headBlock);
                        (*block)->addSucc(predBB);
                        predBB->addPred(*block);
                    }
                    headBlock->addPred(predBB);
                    predBB->addSucc(headBlock);
                    new UncondBrInstruction(headBlock,predBB);
                    for(auto block:pre_block_delete){
                        // std::cout<<block->getNo()<<std::endl;
                        headBlock->removePred(block);
                    }
                    changePhiInstruction(Loop,predBB,pre_block_delete);
                }

                //更改指令间关系
                Instruction* previns=ins->getPrev();
                Instruction* nextins=ins->getNext();

                previns->setNext(nextins);
                nextins->setPrev(previns);

                ins->setParent(predBB);
                predBB->insertBefore(ins,predBB->rbegin());
            }

        }
    }
}

void LoopOptimization::printDomBB(Function * func){
    std::cout<<"domBB:"<<std::endl;
    for(auto block:(func)->getBlockList()){
        std::cout<<block->getNo()<<":";
        for(auto dom : DomBBSet[func][block]){
            std::cout<<dom->getNo()<<" ";
        }
        std::cout<<std::endl;
    }
}

void LoopOptimization::printLoop(std::vector<std::vector<BasicBlock*>>& LoopList){
    for(auto Loop:LoopList){
        std::cout<<"Loop:"<<Loop.size()<<std::endl;
        for(auto block:Loop)
            std::cout<<block->getNo()<<" ";
        std::cout<<std::endl;
    }
}

void LoopOptimization::printLoopConst(std::vector<Instruction*> LoopConstInstructions){
    for(auto ins:LoopConstInstructions){
        ins->output();
    }
}

void LoopOptimization::pass(){
    for(auto func=unit->begin();func!=unit->end();func++){
        calculateFinalDomBBSet(*func);
        std::vector<std::pair<BasicBlock*,BasicBlock*>> BackEdges=getBackEdges(*func);
        std::vector<std::vector<BasicBlock*>> LoopList=calculateLoopList(*func,BackEdges);
        CodePullUp(*func,LoopList,BackEdges);
    }
}




