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
        if(ins->isStore())
            continue;
        if(ins->getDef()==op){
            return true;
        }
    }
    return false;
}
//bin & alloc 
std::vector<Instruction*> LoopOptimization::calculateLoopConstant(std::vector<BasicBlock*> Loop,Function* func){
    std::vector<Instruction*> LoopConstInstructions;
    std::set<Operand*>Operands;
    LoopConst[func][Loop]=Operands;
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
                            if(operand_type->isInt()){
                                IntType* operand_inttype=(IntType*)operand_type;
                                if(operand_inttype->isConst()){
                                    LoopConst[func][Loop].insert(operand);
                                    constant_count++;
                                }
                                //定值点不在循环里,所有给它赋值的语句都在LOOP外面
                                else if(!defInstructionInLoop(operand,Loop)){
                                    LoopConst[func][Loop].insert(operand);
                                    constant_count++;
                                }
                                //只有一个到达定值
                                else if(OperandIsLoopConst(operand,LoopConstInstructions)){
                                    LoopConst[func][Loop].insert(operand);
                                    constant_count++;
                                }
                            }
                            else if(operand_type->isFloat()){
                                FloatType* operand_floattype=(FloatType*)operand_type;
                                if(operand_floattype->isConst()){
                                    LoopConst[func][Loop].insert(operand);
                                    constant_count++;
                                }
                                //定值点不在循环里,所有给它赋值的语句都在LOOP外面
                                else if(!defInstructionInLoop(operand,Loop)){
                                    LoopConst[func][Loop].insert(operand);
                                    constant_count++;
                                }
                                //只有一个到达定值
                                else if(OperandIsLoopConst(operand,LoopConstInstructions)){
                                    LoopConst[func][Loop].insert(operand);
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
                    else if(ins->isAlloc()){//无限制
                        LoopConstInstructions.push_back(ins);
                        LoopConst[func][Loop].insert(ins->getDef());
                        ifAddNew=true;
                    }
                    else if(ins->isGep()){
                        std::vector<Operand*> useOperands = ins->getUse();
                        int constant_count=0;
                        for(auto useOp:useOperands){
                            Type * operand_type=useOp->getEntry()->getType();
                            if(operand_type->isInt()){
                                IntType* operand_inttype=(IntType*)operand_type;
                                if(operand_inttype->isConst()){
                                    LoopConst[func][Loop].insert(useOp);
                                    constant_count++;
                                }
                                //定值点不在循环里,所有给它赋值的语句都在LOOP外面
                                else if(!defInstructionInLoop(useOp,Loop)){
                                    LoopConst[func][Loop].insert(useOp);
                                    constant_count++;
                                }
                                //只有一个到达定值
                                else if(OperandIsLoopConst(useOp,LoopConstInstructions)){
                                    LoopConst[func][Loop].insert(useOp);
                                    constant_count++;
                                }
                            }
                            else if(operand_type->isPtr()){
                                PointerType* operand_ptrtype=(PointerType*)operand_type;
                                if(useOp->isGlobal()){
                                    LoopConst[func][Loop].insert(useOp);
                                    constant_count++;
                                }
                                //定值点不在循环里,所有给它赋值的语句都在LOOP外面
                                else if(!defInstructionInLoop(useOp,Loop)){
                                    LoopConst[func][Loop].insert(useOp);
                                    constant_count++;
                                }
                                //只有一个到达定值
                                else if(OperandIsLoopConst(useOp,LoopConstInstructions)){
                                    LoopConst[func][Loop].insert(useOp);
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
                    else if(ins->isBitcast()){
                        Operand* useOp = ins ->getUse()[0];
                        int constant_count=0;
                        Type * operand_type=useOp->getEntry()->getType();
                        if(operand_type->isPtr()){
                            PointerType* operand_ptrtype=(PointerType*)operand_type;
                            if(useOp->isGlobal()){
                                LoopConst[func][Loop].insert(useOp);
                                constant_count++;
                            }
                            //定值点不在循环里,所有给它赋值的语句都在LOOP外面
                            else if(!defInstructionInLoop(useOp,Loop)){
                                LoopConst[func][Loop].insert(useOp);
                                constant_count++;
                            }
                            //只有一个到达定值
                            else if(OperandIsLoopConst(useOp,LoopConstInstructions)){
                                LoopConst[func][Loop].insert(useOp);
                                constant_count++;
                            }
                        }
                        if(constant_count==1){
                            // ins->output();
                            LoopConstInstructions.push_back(ins);
                            ifAddNew=true;
                        }
                    }
                    else if(ins->isCall()){
                        auto funcSE = (IdentifierSymbolEntry*)(((CallInstruction*)ins)->getFuncSE());
                        if(funcSE->getName() == "llvm.memset.p0.i32"){
                            std::vector<Operand*> useOperands = ins->getUse();
                            Operand* firstOp = useOperands[0];
                            Operand* thirdOp = useOperands[2];
                            if(OperandIsLoopConst(firstOp,LoopConstInstructions)&&(OperandIsLoopConst(thirdOp,LoopConstInstructions)||thirdOp->isConst())){
                                LoopConstInstructions.push_back(ins);
                                ifAddNew=true;
                            }
                        }
                    }
                    else if(ins->isStore()){
                        std::vector<Operand*> useOperands = ins->getUse();
                        Operand* addrOp = useOperands[0];
                        Operand* valueOp = useOperands[1];
                        int constant_count = 0;
                        if(OperandIsLoopConst(addrOp,LoopConstInstructions)||addrOp->isGlobal()||!defInstructionInLoop(addrOp,Loop)){
                            constant_count++;
                        }
                        if(OperandIsLoopConst(valueOp,LoopConstInstructions)||valueOp->isConst()||!defInstructionInLoop(valueOp,Loop)){
                            constant_count++;
                        }
                        if(constant_count==2){
                            LoopConstInstructions.push_back(ins);
                            //store 不增加新的def
                        }
                    }
                    else if(ins->isLoad()){
                        // int constant_count = 0;
                        // Operand* useOp = ins->getUse()[0];
                        // Type * operand_type=useOp->getEntry()->getType();
                        // if(operand_type->isPtr()){
                        //     PointerType* operand_ptrtype=(PointerType*)operand_type;
                        //     if(useOp->isGlobal()){
                        //         LoopConst[func][Loop].insert(useOp);
                        //         constant_count++;
                        //     }
                        //     //定值点不在循环里,所有给它赋值的语句都在LOOP外面
                        //     else if(!defInstructionInLoop(useOp,Loop)){
                        //         LoopConst[func][Loop].insert(useOp);
                        //         constant_count++;
                        //     }
                        //     //只有一个到达定值
                        //     else if(OperandIsLoopConst(useOp,LoopConstInstructions)){
                        //         LoopConst[func][Loop].insert(useOp);
                        //         constant_count++;
                        //     }
                        // }
                        // if(constant_count==1){
                        //     LoopConstInstructions.push_back(ins);
                        //     ifAddNew=true;
                        // }
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
        std::vector<Instruction*> LoopConstInstructions=calculateLoopConstant(Loop,func);
        // printLoopConst(LoopConstInstructions);
        // std::cout<<LoopConstInstructions.size()<<std::endl;
        BasicBlock * headBlock=Loop[0];
        BasicBlock * predBB = new BasicBlock(func);
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

// bool LoopOptimization::ifFindDependLoop(std::vector<BasicBlock*> Loop,Operand* backOperand, Operand* beginOperand,std::set<Operand*> LoopConstList,std::vector<Instruction*>&InstList){
//     for(auto block:Loop){
//         Instruction* i = block->begin();
//         Instruction* last = block->end();
//         Operand* defOp=beginOperand;
//         while (i != last) {
//             if(i->isBin()){
//                 if(backOperand==defOp){
//                     InstList.push_back(i);
//                     return true;
//                 }
//                 std::vector<Operand*> Operands=i->getUse();
//                 if(Operands[0]==defOp){
//                     if(LoopConstList.count(Operands[1])){
//                         InstList.push_back(i);
//                         defOp=i->getDef();
//                     }
//                 }
//                 else if(Operands[1]==defOp){
//                     if(LoopConstList.count(Operands[0])){
//                         InstList.push_back(i);
//                         defOp=i->getDef();
//                     }
//                 }
//             }
//             i=i->getNext();
//         }
//     }
//     return false;
// }  

// bool LoopOptimization::ifInIVList(Operand* op,std::vector<IV_records*> IVs,IV_records* biv){
//     for(auto iv:IVs){
//         if(iv->getTiv()==op){
//             biv=iv;
//             return true;
//         }
//     }
//     return false;
// }

// bool LoopOptimization::ifIV(BinaryInstruction* bin,std::set<Operand*> &LoopConstList,std::vector<IV_records*> IVs,IV_records* iv){
//     std::vector<Operand*> Operands=bin->getUse();
//     IV_records* biv=NULL;
//     if(ifInIVList(Operands[0],IVs,biv)){
//         if(LoopConstList.count(Operands[1])){
//             if(bin->isAdd()){

//             }else if(bin->isSub()){

//             }else if(bin->isMul()){

//             }else if(bin->isDIV()){

//             }
//             return true;
//         }
//         else{
//             return false;
//         }
//     }else if(ifInIVList(Operands[1],IVs,biv)){
//         if(LoopConstList.count(Operands[0])){
//             return true;
//         }
//         else{
//             return false;
//         }
//     }
// }

// std::map<std::vector<BasicBlock*>,std::vector<IV_records*>> LoopOptimization::findInductionVariable(Function* func,std::vector<std::vector<BasicBlock*>>& LoopList){
//     std::map<std::vector<BasicBlock*>,std::vector<IV_records*>> IV_map;
//     for(auto Loop : LoopList){
//         std::set<Operand*> LoopConstList=getLoopConstList(func, Loop);
//         BasicBlock * headBlock=Loop[0];
//         Instruction* i = headBlock->begin();
//         Instruction* last = headBlock->end();
//         while (i != last) {
//             //是phi指令
//             if(i->isPhi()){
//                 PhiInstruction* pi=(PhiInstruction*)i;
//                 for(auto FartherBB=headBlock->succ_begin();FartherBB!=headBlock->succ_end();FartherBB++){
//                     if(count(Loop.begin(), Loop.end(), *FartherBB)){
//                         Operand* backOperand=pi->getSrc(*FartherBB);
//                         Operand* beginOperand=pi->getDef();
//                         std::vector<Instruction*>InstList;
//                         if(ifFindDependLoop(Loop,backOperand,beginOperand,LoopConstList,InstList)){
//                             IV_records* iv=new IV_records(*FartherBB,pi,beginOperand,beginOperand,1,0);
//                             IV_map[Loop].push_back(iv);
//                         }
//                     }
//                 }
//             }
//             i = i->getNext();
//         }
//         //already find BIV
//         bool change=false;
//         do{
//             for(auto block:Loop){
//                 Instruction* i = block->begin();
//                 Instruction* last = block->end();
//                 while (i != last) {
//                     if(i->isBin()){
//                         BinaryInstruction* bin=(BinaryInstruction*)i;
//                         IV_records *iv=NULL;
//                         if(ifIV(bin,LoopConstList,IV_map[Loop],iv)){
                            
//                         }
//                     }

//                     i=i->getNext();
//                 }
//             }
//         }while(change);
//     }
//     return IV_map;
// }

Operand* LoopOptimization::replaceCmpIns(Instruction* cmpIns,reduceOperate* reduceOp,Operand* useOp){
    Function* func=cmpIns->getParent()->getParent();
    Operand* op=reduceOp->getop();
    //更改useop
    if(useOp->getEntry()->isConstant()){
        int val1=((ConstantSymbolEntry*)useOp->getEntry())->getValue();
        if(op->getEntry()->isConstant()){
            int val;
            int val2=((ConstantSymbolEntry*)op->getEntry())->getValue();
            switch (reduceOp->getOpCode()) {
                case SSAGraphNode::SUB:
                    val = val1 - val2;
                    break;
                case SSAGraphNode::ADD:
                    val = val1 + val2;
                    break;
                case SSAGraphNode::AND:
                    val = val1 && val2;
                    break;
                case SSAGraphNode::OR:
                    val = val1 || val2;
                    break;
                case SSAGraphNode::MUL:
                    val = val1 * val2;
                    break;
                case SSAGraphNode::DIV:
                    val = val1 / val2;
                    break;
                case SSAGraphNode::MOD:
                    val = val1 % val2;
                    break;
            }
            Operand* replaceOp=new Operand(new ConstantSymbolEntry(TypeSystem::intType,val));
            // cmpIns->replaceUse(final,replaceOp);
            return replaceOp;
        }
    }
    Operand* newDefOp=new Operand(new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel()));
    int binOpcode;
    switch (reduceOp->getOpCode()) {
        case SSAGraphNode::SUB:
            binOpcode=BinaryInstruction::SUB;
            break;
        case SSAGraphNode::ADD:
            binOpcode=BinaryInstruction::ADD;
            break;
        case SSAGraphNode::AND:
            binOpcode=BinaryInstruction::AND;
            break;
        case SSAGraphNode::OR:
            binOpcode=BinaryInstruction::OR;
            break;
        case SSAGraphNode::MUL:
            binOpcode=BinaryInstruction::MUL;
            break;
        case SSAGraphNode::DIV:
            binOpcode=BinaryInstruction::DIV;
            break;
        case SSAGraphNode::MOD:
            binOpcode=BinaryInstruction::MOD;
            break;
        default:
            assert("opcode don't exist");
    }
    BinaryInstruction* bin= new BinaryInstruction(binOpcode,newDefOp,useOp,op,nullptr);
    Instruction* defIns=useOp->getDef();
    BasicBlock* insertBB=defIns->getParent();
    insertBB->insertBefore(bin,defIns->getNext());
    
    // cmpIns->replaceUse(final,newDefOp);
    return newDefOp;
    // final1=reduceOp->getnew();
}

void LoopOptimization::LFTR(std::vector<BasicBlock*>& loop, std::vector<Instruction*> CmpInsList,Function* func){
    for(auto cmpIns:CmpInsList){
        
        BasicBlock* fartherBB=cmpIns->getParent();
        int num=0;
        std::vector<Operand*>useOperands;
        for(auto useOp:cmpIns->getUse()){
            useOperands.push_back(useOp);
        }

        for(auto useOp:useOperands){
            int replacenum;
            bool change=false;
            Operand* final=useOp;
            Operand* replaceOp;
            if(num==0){
                replaceOp=cmpIns->getUse()[1];
                replacenum=1;
            }
            else{
                replaceOp=cmpIns->getUse()[0];
                replacenum=0;
            }
            for(auto reduceOp:reduceTable[func]){
                // std::cout<<reduceOp->getold()->toStr()<<" "<<reduceOp->getnew()->toStr()<<std::endl;
                if(reduceOp->getold()==final){
                    replaceOp=replaceCmpIns(cmpIns,reduceOp,replaceOp);
                    final=reduceOp->getnew();
                    change=true;
                }
            }
            if(change){
                cmpIns->replaceUse(useOp,final);
                cmpIns->replaceUse(cmpIns->getUse()[replacenum],replaceOp);
            }
            
            num++;
        }

    }
}

std::vector<SSAGraphNode*> LoopOptimization::getLoopSSAGraph(std::vector<BasicBlock*>Loop,std::vector<SSAGraphNode*>Graph,std::vector<SSAGraphNode*>& NotCandidateList,std::vector<Instruction*>&CmpInsList){
    //如果是const可以保留，如果不是const则得去掉
    std::vector<SSAGraphNode*> loopGraph;
    for(auto block:Loop){
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            if(in->isCmp()){
                CmpInsList.push_back(in);
            }
            SSAGraphNode* node=in->getNode();
            if(node!=nullptr&&!count(loopGraph.begin(),loopGraph.end(),node))
                loopGraph.push_back(node);
        }
    }
    
    for(auto node:loopGraph){
        std::vector<SSAGraphNode*> children=node->getChildren();
        for(auto child:children){
            if(!count(loopGraph.begin(),loopGraph.end(),child)){
                NotCandidateList.push_back(child);
            }
        }
    }
    
    for(auto tempnode:NotCandidateList){
        loopGraph.push_back(tempnode);
    }

    return loopGraph;
}


int nextnum=0;
int c;


void LoopOptimization::OSR(std::vector<SSAGraphNode*> graph,Function* func,std::vector<std::vector<BasicBlock*>> LoopList){

    //每次循环结束后要清空所有的节点信息，例如visit,header,low,num等等
    for(auto loop:LoopList){
        std::vector<SSAGraphNode*> NotCandidateList;
        std::vector<Instruction*>CmpInsList;
        std::vector<SSAGraphNode*> loopGraph=getLoopSSAGraph(loop,graph,NotCandidateList,CmpInsList);
        std::vector<SSAGraphNode*> NodeStack;
        std::map<IV_record*,Operand*>NodeHashTable;
        nextnum=0;
        c=0;
        for(auto node:loopGraph){
            if(!node->isvisit()){
                DFS(node,NodeStack,NodeHashTable,func,NotCandidateList);
            }
        }

        for(auto node:loopGraph){
            node->resetvisit();
            node->resetHeader();
        }
        // LFTR(loop,CmpInsList,func);
    }
}

void LoopOptimization::DFS(SSAGraphNode* n,std::vector<SSAGraphNode*>& NodeStack,std::map<IV_record*,Operand*>&NodeHashTable,Function* func,std::vector<SSAGraphNode*> NotCandidateList){
    
    n->setnum(nextnum++);
    n->setvist();
    n->setlow(n->getnum());
    // std::cout<<"DFS"<<std::endl;
    // std::cout<<"num:"<<n->getnum()<<std::endl;
    // if(n->getType()==SSAGraphNode::CONST){
    //     std::cout<<"const:";
    // }
    // std::cout<<(n->getOperator()->toStr())<<std::endl;
    NodeStack.push_back(n);
    if(!count(NotCandidateList.begin(),NotCandidateList.end(),n)){
        for(auto child:n->getChildren()){
            if(!child->isvisit()){
                DFS(child,NodeStack,NodeHashTable,func,NotCandidateList);
                n->setlow(min(n->getlow(),child->getlow()));
            }
            if(n->getnum()>child->getnum()&&count(NodeStack.begin(),NodeStack.end(),child)){
                n->setlow(min(n->getlow(),child->getnum()));
            }
        }
    }
    
    if(n->getlow()==n->getnum()){
        std::vector<SSAGraphNode*> SCC;
        SSAGraphNode* x=nullptr;
        while(x!=n){
            x=NodeStack[NodeStack.size()-1];
            NodeStack.pop_back();
            SCC.push_back(x);
        }
        // std::cout<<"scc"<<std::endl;
        // printSCC(SCC);
        // std::cout<<"scc end"<<std::endl;
        Process(SCC,NodeHashTable,func,NotCandidateList);
    }
    // std::cout<<"DFS ok"<<std::endl;
}

bool LoopOptimization::isCandidate(SSAGraphNode* n,SSAGraphNode* &iv,SSAGraphNode* &rc,std::vector<SSAGraphNode*> NotCandidateList){
    if(count(NotCandidateList.begin(),NotCandidateList.end(),n)){
        return false;
    }
    if(n->getType()==SSAGraphNode::CONST||n->getType()==SSAGraphNode::GLOBAL||n->getType()==SSAGraphNode::CALL){
        return false;
    }
    std::vector<SSAGraphNode*> children=n->getChildren();
    if(children.size()==1){
        return false;
    }
    SSAGraphNode* first=children[0];
    SSAGraphNode* second=children[1];
    if(n->getType()==SSAGraphNode::SUB){
        if(first->getHeader()!=nullptr&&first->getHeader()!=first->getorigin()&&isLocalVar(first,second)){
            iv=children[0];
            rc=children[1];
            return true;
        }
    }
    else if(n->getType()==SSAGraphNode::ADD||n->getType()==SSAGraphNode::MUL){
        if((first->getHeader()!=nullptr&&first->getHeader()!=first->getorigin()&&isLocalVar(first,second))){
            iv=children[0];
            rc=children[1];
            return true;
        }
        else if((second->getHeader()!=nullptr&&second->getHeader()!=second->getorigin()&&isLocalVar(second,first))){
            iv=children[1];
            rc=children[0];
            return true;
        }
    }
    return false;
}

bool LoopOptimization::isLocalVar(SSAGraphNode* i,SSAGraphNode* c){
    if(c->getType()==SSAGraphNode::CONST){
        return true;
    }
    //c BB dom i BB
    else{
        BasicBlock* c_Farther=c->getDefBB();
        BasicBlock* i_Farther=i->getDefBB();
        std::vector<BasicBlock*> Dom_i_Farther=DomBBSet[i_Farther->getParent()][i_Farther];
        if(count(Dom_i_Farther.begin(),Dom_i_Farther.end(),c_Farther)){
            return true;
        }
    }
    return false;
}

bool LoopOptimization::isValidUpdate(SSAGraphNode* n){
    int type=n->getType();
    if(type==SSAGraphNode::PHI){
        return true;
    }
    std::vector<SSAGraphNode*> children=n->getChildren();
    SSAGraphNode* first=children[0];
    SSAGraphNode* second=children[1];
    if(type==SSAGraphNode::SUB){
        if(first->getHeader()!=nullptr&&isLocalVar(first,second)){
            return true;
        }
    }
    else if(type==SSAGraphNode::ADD){
        if((first->getHeader()!=nullptr&&isLocalVar(first,second))||(second->getHeader()!=nullptr&&isLocalVar(second,first))){
            return true;
        }
    }
    return false;
}

void LoopOptimization::Process(std::vector<SSAGraphNode*>& SCC,std::map<IV_record*,Operand*>&NodeHashTable,Function* func,std::vector<SSAGraphNode*> NotCandidateList){
    if(SCC.size()==1){
        SSAGraphNode* n=SCC[0];
        SSAGraphNode*iv;
        SSAGraphNode*rc;
        if(isCandidate(n,iv,rc,NotCandidateList)){
            //replace
            // std::cout<<"can"<<std::endl;
            replace(n,iv,rc,NodeHashTable,func);
            // n->setHeader(iv->getHeader());
        }
        else{
            // std::cout<<"null"<<std::endl;
            n->setHeader(nullptr);
        }
    }
    //classify
    else{
        // std::cout<<"classify"<<std::endl;
        ClassifyIV(SCC,NodeHashTable,func,NotCandidateList);
    }
    // std::cout<<"pro end"<<std::endl;
}


void LoopOptimization::search(SSAGraphNode* n,std::map<SSAGraphNode*,bool>& visit,std::map<SSAGraphNode*,int>& dfn){
    visit[n]=true;
    std::vector<SSAGraphNode*>children=n->getChildren();
    for(auto child:children){
        if(!visit[child]){
            search(child,visit,dfn);
        }
    }
    dfn[n]=c;
    c=c-1;
}

SSAGraphNode* LoopOptimization::getRPONode(std::vector<SSAGraphNode*>& SCC){
    std::map<SSAGraphNode*,bool> visit;
    std::map<SSAGraphNode*,int> dfn;
    for(auto node:SCC){
        visit[node]=false;
    }
    c=SCC.size();
    SSAGraphNode* entry=SCC[SCC.size()-1];
    if(entry->getlow()!=entry->getnum()){
        assert("entry error");
    }
    search(entry,visit,dfn);
    int min=SCC.size()+1;
    SSAGraphNode* PRONode;
    for(auto item:dfn){
        if(item.second<min){
            min=item.second;
            PRONode=item.first;
        }
    }
    return PRONode;
}

void LoopOptimization::ClassifyIV(std::vector<SSAGraphNode*>& SCC,std::map<IV_record*,Operand*>&NodeHashTable,Function* func,std::vector<SSAGraphNode*> NotCandidateList){
    bool IsIV=true;
    SSAGraphNode* header=getRPONode(SCC);
    for(auto node:SCC){
        if(!isValidUpdate(node)){
            IsIV=false;
        }
        // if(node->getType()!=SSAGraphNode::ADD||node->getType()!=SSAGraphNode::SUB||node->getType()!=SSAGraphNode::PHI){
        //     IsIV=false;
        // }
        // else{
        //     for(auto child:node->getChildren()){
        //         if(!count(SCC.begin(),SCC.end(),child)&&!isLocalVar(header,child)){
        //             IsIV=false;
        //         }
        //     }
        // }
    }
    if(IsIV){
        // std::cout<<"iv"<<std::endl;
        for(auto node:SCC){
            node->setHeader(header);
        }
    }
    else{
        // std::cout<<"else iv"<<std::endl;
        for(auto node:SCC){
            SSAGraphNode* iv;
            SSAGraphNode* rc;
            if(isCandidate(node,iv,rc,NotCandidateList)){
                //replace
                replace(node,iv,rc,NodeHashTable,func);
                // node->setHeader(iv->getHeader());
            }
            else{
                node->setHeader(nullptr);
            }
        }
    }
}

Operand* LoopOptimization::ifInHashTable(std::map<IV_record*,Operand*>&NodeHashTable,IV_record* iv_record){
    for(auto item:NodeHashTable){
        IV_record* temp_iv=item.first;
        if(temp_iv->getiv()==iv_record->getiv()&&temp_iv->getrc()==iv_record->getrc()&&temp_iv->getOp()==iv_record->getOp()){
            return item.second;
        }
    }
    return nullptr;
}
//已经复制了def
void LoopOptimization::rewrite(Operand* _new,Operand* _old,Function* func){
    for(auto block:func->getBlockList()){
        for(auto in = block->begin(); in != block->end(); in = in->getNext()){
            //理论上不可能有
            // if(in->getDef()==_old){
            //     in->replaceDef(_new);
            // }
            for(auto useOp:in->getUse()){
                if(useOp==_old){
                    in->replaceUse(_old,_new);
                }
            }
        }
    }
}

Operand* LoopOptimization::apply(IV_record* iv,std::map<IV_record*,Operand*>&NodeHashTable,Function* func){
    Operand* result=ifInHashTable(NodeHashTable,iv);
    if(result==nullptr){
        // std::cout<<"result null"<<std::endl;
        SSAGraphNode*first=iv->getiv();
        SSAGraphNode*second=iv->getrc();
        if((first->getHeader()!=nullptr&&isLocalVar(first,second))){
            result=reduce(iv,NodeHashTable,func);
        }
        else if((second->getHeader()!=nullptr&&isLocalVar(second,first))){
            IV_record* tempIV=new IV_record(second,first,iv->getOp());
            result=reduce(tempIV,NodeHashTable,func);
        }
        else{
            // std::cout<<"apply else"<<std::endl;
            Operand* defOp=iv->getIVOp();
            Type* type=defOp->getType();
            result=new Operand(new TemporarySymbolEntry(type,SymbolTable::getLabel()));
            // std::cout<<"new result"<<std::endl;
            // std::cout<<"get bb"<<std::endl;
            BasicBlock* first_bb=first->getDefBB();
            BasicBlock* second_bb=second->getDefBB();
            int binOpcode;
            switch (iv->getOp()) {
                case SSAGraphNode::SUB:
                    binOpcode=BinaryInstruction::SUB;
                    break;
                case SSAGraphNode::ADD:
                    binOpcode=BinaryInstruction::ADD;
                    break;
                case SSAGraphNode::AND:
                    binOpcode=BinaryInstruction::AND;
                    break;
                case SSAGraphNode::OR:
                    binOpcode=BinaryInstruction::OR;
                    break;
                case SSAGraphNode::MUL:
                    binOpcode=BinaryInstruction::MUL;
                    break;
                case SSAGraphNode::DIV:
                    binOpcode=BinaryInstruction::DIV;
                    break;
                case SSAGraphNode::MOD:
                    binOpcode=BinaryInstruction::MOD;
                    break;
                default:
                    assert("bin type not exist");
            }
            //rc_bb dom iv_bb
            if(first_bb!=nullptr&&second_bb!=nullptr){
                std::vector<BasicBlock*> firstList=DomBBSet[func][first_bb];
                std::vector<BasicBlock*> secondList=DomBBSet[func][second_bb];
                std::vector<BasicBlock*> mergeList=getIntersectBBList(firstList,secondList);
                BasicBlock* insertBB=mergeList[0];
                BinaryInstruction* bin= new BinaryInstruction(binOpcode, result, first->getOperator(), second->getOperator(), nullptr);
                insertBB->insertBefore(bin,insertBB->rbegin());
                //产生新的SSAGraphNode
                bin->genNode();
                bin->getNode()->setHeader(nullptr);
                func->addNodeToSSAGraph(bin->getNode());
            }
            else if(first_bb!=nullptr&&second_bb==nullptr){
                BasicBlock* insertBB=first_bb;
                BinaryInstruction* bin= new BinaryInstruction(binOpcode, result, first->getOperator(), second->getOperator(), nullptr);
                insertBB->insertBefore(bin,insertBB->rbegin());
                //产生新的SSAGraphNode
                bin->genNode();
                bin->getNode()->setHeader(nullptr);
                func->addNodeToSSAGraph(bin->getNode());
            }
            else if(second_bb!=nullptr&&first_bb==nullptr){
                BasicBlock* insertBB=second_bb;
                BinaryInstruction* bin= new BinaryInstruction(binOpcode, result, first->getOperator(), second->getOperator(), nullptr);
                insertBB->insertBefore(bin,insertBB->rbegin());
                //产生新的SSAGraphNode
                bin->genNode();
                bin->getNode()->setHeader(nullptr);
                func->addNodeToSSAGraph(bin->getNode());
            }
            else if(first_bb==nullptr&&second_bb==nullptr){
                int val1=first->getvalue();
                int val2=second->getvalue();
                int val;
                switch (iv->getOp()) {
                    case SSAGraphNode::SUB:
                        val = val1 - val2;
                        break;
                    case SSAGraphNode::ADD:
                        val = val1 + val2;
                        break;
                    case SSAGraphNode::AND:
                        val = val1 && val2;
                        break;
                    case SSAGraphNode::OR:
                        val = val1 || val2;
                        break;
                    case SSAGraphNode::MUL:
                        val = val1 * val2;
                        break;
                    case SSAGraphNode::DIV:
                        val = val1 / val2;
                        break;
                    case SSAGraphNode::MOD:
                        val = val1 % val2;
                        break;
                }
                result = new Operand(new ConstantSymbolEntry(TypeSystem::intType,val));
            }
        }
    }
    reduceOperate* reOp=new reduceOperate(result,iv->getiv()->getOperator(),iv->getOp(),iv->getrc()->getOperator());
    reduceTable[func].push_back(reOp);
    NodeHashTable[iv]=result;
    return result;
}

Operand* LoopOptimization::reduce(IV_record* iv_record,std::map<IV_record*,Operand*>&NodeHashTable,Function* func){
    Operand* result=ifInHashTable(NodeHashTable,iv_record);
    //not found
    // std::cout<<"reduce"<<std::endl;
    if(result==nullptr){
        // std::cout<<"reduce nullptr"<<std::endl;
        Operand* defOp=iv_record->getIVOp();
        Type* type=defOp->getType();
        SymbolEntry* se=defOp->getEntry();
        if(se->isTemporary()){
            result=new Operand(new TemporarySymbolEntry(type,SymbolTable::getLabel()));
        }
        else if(se->isConstant()){
            assert("iv will not be const");
            // result=new Operand(new ConstantSymbolEntry(type));
        }
        else if(se->isVariable()){
            assert("iv will not be var");
        }
             
        IV_record* iv=new IV_record(iv_record->getiv(),iv_record->getrc(),iv_record->getOp());
        reduceOperate* reOp=new reduceOperate(result,iv_record->getiv()->getOperator(),iv_record->getOp(),iv_record->getrc()->getOperator());
        reduceTable[func].push_back(reOp);

        NodeHashTable[iv]=result;


        // SSAGraphNode* newDef=iv_record->getiv();
        // newDef->getIns()->replaceDef(result);

        SSAGraphNode* newDef=iv_record->getiv()->IV_clone(result);
        func->addNodeToSSAGraph(newDef);



        std::vector<SSAGraphNode*> newDefChildrenList;
        newDefChildrenList.assign(newDef->getChildren().begin(),newDef->getChildren().end());
        // std::cout<<"reduce new result"<<std::endl;   
        int num=0;
        for(auto child:newDefChildrenList){
            if(child->getHeader()==newDef->getHeader()){
                // std::cout<<"header equal"<<std::endl;   
                IV_record* temp_iv=new IV_record(child,iv_record->getrc(),iv_record->getOp());
                Operand* _old=newDef->getIns()->getUse()[num];
                Operand* _new=reduce(temp_iv,NodeHashTable,func);
                newDef->getIns()->replaceUse(_old,_new);
                newDef->removeChild(child);
                if(_new->getEntry()->isConstant()){
                    SSAGraphNode* newchild=new SSAGraphNode(((ConstantSymbolEntry*)_new->getEntry())->getValue());
                    newchild->setvist();
                    newchild->setHeader(nullptr);
                    newDef->addChild(newchild);
                }
                else{//不是常数的header已经被设置过了
                    newDef->addChild(_new->getDef()->getNode());
                }
                // rewrite(_new,_old,func);
                // std::cout<<"rewrite done"<<std::endl;   
            }
            else if(iv_record->getOp()==SSAGraphNode::MUL||newDef->getType()==SSAGraphNode::PHI){
                // std::cout<<"mul phi"<<std::endl;
                IV_record* temp_iv=new IV_record(child,iv_record->getrc(),iv_record->getOp());
                // std::cout<<"apply"<<std::endl;
                Operand* _old=newDef->getIns()->getUse()[num];
                Operand* _new=apply(temp_iv,NodeHashTable,func);
                newDef->getIns()->replaceUse(_old,_new);
                newDef->removeChild(child);
                if(_new->getEntry()->isConstant()){
                    SSAGraphNode* newchild=new SSAGraphNode(((ConstantSymbolEntry*)_new->getEntry())->getValue());
                    newchild->setvist();
                    newchild->setHeader(nullptr);
                    newDef->addChild(newchild);
                }
                else{
                    SSAGraphNode* newchild=_new->getDef()->getNode();
                    newDef->addChild(newchild);
                    // newchild->setvist();
                }

                // rewrite(apply(temp_iv,NodeHashTable,func),newDef->getIns()->getUse()[count],func);
            }
            // std::cout<<"child"<<std::endl;
            num++;
        }

    }
    return result;
    // std::cout<<"reduce end"<<std::endl;
}

void LoopOptimization::replace(SSAGraphNode* n,SSAGraphNode* iv,SSAGraphNode* rc,std::map<IV_record*,Operand*>&NodeHashTable,Function* func){
    // std::cout<<"replace"<<std::endl;
    IV_record* iv_record=new IV_record(iv,rc,n->getType());

    Operand* result=reduce(iv_record,NodeHashTable,func);
    // std::cout<<"replace reduce done"<<std::endl;
    BinaryInstruction* copy=new BinaryInstruction(BinaryInstruction::ADD,n->getOperator(),result,new Operand(new ConstantSymbolEntry(TypeSystem::intType,0)),nullptr);
    BasicBlock* fartherBB=n->getIns()->getParent();
    Instruction* succIns=n->getIns()->getNext();
    fartherBB->remove(n->getIns());
    fartherBB->insertBefore(copy,succIns);
    copy->setNode(n);
    n->setIns(copy);
    n->setHeader(iv->getHeader());

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

void LoopOptimization::printSCC(std::vector<SSAGraphNode*> SCC){
    for(auto node:SCC){
        std::cout<<node->getOperator()->toStr()<<" ";
    }
    std::cout<<std::endl;
}

// void printIV(std::vector<IV_records*> IVs){
//     for(auto iv:IVs){
//         Operand* tiv=iv->getTiv();
//         std::string a=tiv->toStr();
//         std::cout<<a.c_str()<<std::endl;
//     }
// }

void LoopOptimization::dealwithNoPreBB(Function* func){
    std::vector<BasicBlock*> temp;
    std::vector<BasicBlock*> blocklist = func -> getBlockList();
    for (auto it = blocklist.begin(); it != blocklist.end(); it++) {
        if ((*it)->getNumOfPred() == 0 && *it != func->getEntry()) {
            for (auto it1 = (*it)->pred_begin(); it1 != (*it)->pred_end();
                 it1++) {
                (*it1)->removePred(*it);
            }
            temp.push_back(*it);
            // ret = true;
        }
    }
    for (auto b : temp)
        func->remove(b);
}

void LoopOptimization::pass(){
    for(auto func=unit->begin();func!=unit->end();func++){
        calculateFinalDomBBSet(*func);
        std::vector<std::pair<BasicBlock*,BasicBlock*>> BackEdges=getBackEdges(*func);
        std::vector<std::vector<BasicBlock*>> LoopList=calculateLoopList(*func,BackEdges);
        // printLoop(LoopList);
        CodePullUp(*func,LoopList,BackEdges);
        dealwithNoPreBB(*func);
        //外提后会产生新的块 需要重新计算循环
        calculateFinalDomBBSet(*func);
        BackEdges=getBackEdges(*func);
        LoopList=calculateLoopList(*func,BackEdges);
        // (*func)->genSSAGraph();
        // printLoop(LoopList);
        //strength reduction
        // (*func)->genSSAGraph();
        // OSR((*func)->getSSAGraph(),*func,LoopList);
        // auto& blocklist = func->getBlockList();

        LoopUnroll Ln;
        Ln.setDomBBset(DomBBSet);
        std::vector<std::vector<BasicBlock*>> InnerLoopList = Ln.calculateInnerLoop(LoopList);
        Ln.findLoop(InnerLoopList);
        Ln.Unroll();
    }
}

void LoopOptimization::pass1(){
    for(auto func=unit->begin();func!=unit->end();func++){
        calculateFinalDomBBSet(*func);
        std::vector<std::pair<BasicBlock*,BasicBlock*>> BackEdges=getBackEdges(*func);
        std::vector<std::vector<BasicBlock*>> LoopList=calculateLoopList(*func,BackEdges);
        // printLoop(LoopList);
        CodePullUp(*func,LoopList,BackEdges);
        dealwithNoPreBB(*func);
    }
}



