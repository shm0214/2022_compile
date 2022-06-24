#include "LoopOptimization.h"
#include <algorithm>

//after ssa
bool LoopOptimization :: ifDomBBSetChange(std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>& lastSet){
    for(auto block: func->getBlockList()){
        if(lastSet[block]!=getDomBBSet()[block]){
            return true;
        }
    }
    return false;
}

void LoopOptimization :: initializeDomBBSet(){
    for (auto block : func->getBlockList()) {
        if(DomBBSet.find(block)==DomBBSet.end()){
            if(block==func->getEntry()){
                std::vector<BasicBlock*> dom_list;
                dom_list.push_back(func->getEntry());
                DomBBSet[block]=dom_list;
            }
            else{
                std::vector<BasicBlock*> dom_list(func->getBlockList());
                DomBBSet[block]=dom_list;
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

void LoopOptimization::calculateFinalDomBBSet(){
    initializeDomBBSet();
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> lastSet(getDomBBSet());
    while(true){
        std::unordered_map<BasicBlock*,std::vector<BasicBlock*>>DomSet=getDomBBSet();
        for (auto block: func->getBlockList()){
            std::vector<BasicBlock*> finalDomList=DomSet[*(block->pred_begin())];
            
            for (auto fartherBlock=++block->pred_begin();fartherBlock<block->pred_end();fartherBlock++){
                finalDomList=getIntersectBBList(DomSet[*fartherBlock],finalDomList);
            }
            if(!count(finalDomList.begin(),finalDomList.end(),block)){
                finalDomList.push_back(block);
            }
            DomSet[block]=finalDomList;
        }

        if(!ifDomBBSetChange(lastSet)){
            break;
        }
        else{
            lastSet=getDomBBSet();
        }
    }

    return;
}

std::vector<std::vector<std::pair<BasicBlock*,BasicBlock*>>> LoopOptimization::mergeEdge(std::vector<std::pair<BasicBlock*,BasicBlock*>> BackEdges){
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

void LoopOptimization::calculateLoopList(){
    std::vector<std::pair<BasicBlock*,BasicBlock*>> BackEdges;
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> DomSet=getDomBBSet();
    //search for backedges
    for (auto block:func->getBlockList()){
        for (auto domBB:DomSet[block]){
            for(auto succ_block=block->succ_begin();succ_block<block->succ_end();succ_block++){
                if(*succ_block==domBB){
                    std::pair<BasicBlock*,BasicBlock*> edge(block,domBB);
                    BackEdges.push_back(edge);
                }
            }
        }
    }
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
}

bool LoopOptimization::InstructionInLoop(Instruction* ins,std::vector<BasicBlock*>Loop){
    for(auto block:Loop){
        Instruction* i = block->begin();
        Instruction* last = block->end();
        while (i != last) {
            if(i==ins){
                return true;
            }
            i = i->getNext();
        }
    }
    return false;
}

bool LoopOptimization::OperandIsLoopConst(Operand * op,std::vector<Instruction*> LoopConstInstructions){
    for(auto ins:LoopConstInstructions){
        if(ins->getDef()==op){
            return true;
        }
    }
    return false;
}

std::vector<Instruction*> LoopOptimization::calculateLoopConstant(std::vector<BasicBlock*> Loop){
    std::vector<Instruction*> LoopConstInstructions;
    for(auto block:Loop){
        while(true){
            Instruction* ins = block->begin();
            Instruction* last = block->end();
            bool ifAddNew=false;
            while (ins != last) {
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
                            //定值点不在循环里
                            else if(!InstructionInLoop(operand->getDef(),Loop)){
                                constant_count++;
                            }
                            //定值点在循环里，但是该语句被标记为不变
                            else if(OperandIsLoopConst(operand,LoopConstInstructions)){
                                constant_count++;
                            }
                        }
                    }
                    if(constant_count==2){
                        LoopConstInstructions.push_back(ins);
                        ifAddNew=true;
                    }
                }
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


void LoopOptimization::CodePullUp(){
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> domBBSet=getDomBBSet();
    for(auto Loop:LoopList){
        std::vector<BasicBlock*> outBlock=calculateOutBlock(Loop);
        std::vector<Instruction*> LoopConstInstructions=calculateLoopConstant(Loop);
        BasicBlock * headBlock=Loop[0];
        BasicBlock * predBB=new BasicBlock(getfunc());
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
            //可以外提
            if(if_DomBBAll){
                //设置predBB的跳转，以及BasciBlock中的跳转指令的目的块
                if(ifAddFirst){
                    ifAddFirst=false;
                    //查最后一条指令然后改掉
                    for(auto block=headBlock->pred_begin();block!=predBB->pred_end();block++){
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
                    predBB->addSucc(headBlock);
                    new UncondBrInstruction(headBlock,predBB);
                }
                
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





