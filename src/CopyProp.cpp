#include "CopyProp.h"
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

vector<unordered_map<SymbolEntry*, SymbolEntry*>> copy, kill, cpin, cpout;

void remove_ACP(unordered_map<SymbolEntry*, SymbolEntry*> ACP, SymbolEntry* sym){
    for(auto iter = ACP.begin();iter!=ACP.end();){
        if(iter->first==sym || iter->second==sym){
            ACP.erase(iter++);
        }
        else{
            ++iter;
        }
    }
}

SymbolEntry* copy_value(Operand* opd, unordered_map<SymbolEntry*, SymbolEntry*> ACP){
    SymbolEntry* sym = opd->getEntry();
    bool isVar = opd->getEntry()->isVariable();
    for(auto iter = ACP.begin(); iter!=ACP.end();iter++){
        SymbolEntry* key = iter->first;
        SymbolEntry* value = iter->second;
        if(isVar && sym == key){
            return value;
        }
    }
    return sym;
}

void CopyProp::copy_prop(){
    auto iter = unit->begin();
    unordered_map<SymbolEntry*, SymbolEntry*> ACP;
    while (iter != unit->end()){
        vector<BasicBlock*> block_list = (*iter++)->getBlockList();
        BasicBlock* bb;
        for(int i=0;i<block_list.size();i++){
            // find the first block(has no preds)
            bb = block_list[i];
            if(bb->predEmpty()){
                break;
            }
        }
        queue<BasicBlock*> q;
        q.push(bb);
        bool first = true;
        while(!q.empty()){
            BasicBlock* bb = q.front();
            int no = bb->getNo();
            if(first){
                // cpin[no] = none
            }
            else{
                //cpin[no] = 
            }
            local_copy_prop(bb);
            for(auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++){

            }
        }
    }
}

void CopyProp::local_copy_prop(BasicBlock* bb){
    int no = bb->getNo();
    enum {
        BINARY,
        COND,
        UNCOND,
        RET,
        LOAD,
        STORE,
        CMP,
        ALLOCA,
        CALL,
        ZEXT,
        XOR,
        GEP
    };
    unordered_map<SymbolEntry*, SymbolEntry*> ACP;
    for(auto iter = bb->begin();iter!=bb->end();iter++){
        int kind = iter->getInstType();
        vector<Operand*> operands = iter->getOperands();
        if (kind == BINARY)
        {
            SymbolEntry* sym1 = copy_value(operands[1], ACP);
            SymbolEntry* sym2 = copy_value(operands[2], ACP);
            operands[1]->setEntry(sym1);
            operands[2]->setEntry(sym2);
        }
        else{
            if(kind == LOAD){
                SymbolEntry* sym1 = operands[0]->getEntry();
                remove_ACP(ACP, sym1);
                if(operands[1]->getEntry()->isVariable()){
                    SymbolEntry* sym2 = operands[1]->getEntry();
                    ACP[sym1] = sym2;
                }
            }
        }
    }
    //copy.push_back(ACP);
}



void CopyProp::global_copy_prop(BasicBlock* bb){

}


