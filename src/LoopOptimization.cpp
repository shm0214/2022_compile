#include "LoopOptimization.h"
#include <algorithm>


bool LoopOptimization :: ifDomBBSetChange(std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> lastSet){
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

std::vector<BasicBlock*> LoopOptimization::getIntersectBBList(std::vector<BasicBlock*> List1,std::vector<BasicBlock*>List2){
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

void LoopOptimization::calculateLoopList(){
    std::vector<std::pair<BasicBlock*,BasicBlock*>> BackEdges;
    std::unordered_map<BasicBlock*,std::vector<BasicBlock*>> DomSet=getDomBBSet();
    //search for backedges
    for (auto block:func->getBlockList()){
        for (auto domBB:DomSet[block]){
            for(auto succ_block=block->succ_begin();succ_block<block->succ_end();succ_block++){
                if(*succ_block==domBB)
                    std::pair<BasicBlock*,BasicBlock*> edge(block,domBB);
            }
        }
    }
    //search for natural loop
    for (auto edge:BackEdges){
        std::vector<BasicBlock*> Lastadd;
        std::vector<BasicBlock*> Loop;
        if(edge.first!=edge.second){
            Loop.push_back(edge.first);
            Loop.push_back(edge.second);
            Lastadd.push_back(edge.first);
            // Lastadd.push_back(edge.second);
        }
        else{
            Loop.push_back(edge.first);
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
        LoopList.push_back(Loop);
    }
}

std::vector<Instruction*> calculateLoopConstant(std::vector<BasicBlock*> Loop){
    std::vector<Instruction*> pullOutInstructions;
    for(auto block:Loop){
        
    }
}


void LoopOptimization::CodePullUp(){


}





