#include "CopyProp.h"
#include <vector>
#include <set>
#include <queue>
using namespace std;
const int N = 2000100;
typedef vector<BasicBlock*>::iterator bb_iterator;
bool st[N]; // 标记基本块是否已遍历过

unordered_map<int, USS> COPY, KILL, CPin, CPout, load_sym;
unordered_map<int, USO> opds;

void remove_ACP(USS &ACP, SymbolEntry* sym){
    for(auto iter = ACP.begin();iter!=ACP.end();){
        if(iter->first==sym || iter->second==sym){
            ACP.erase(iter++);
        }
        else{
            ++iter;
        }
    }
}

SymbolEntry* copy_value(Operand* opd, USS ACP, bool &flag){
    SymbolEntry* sym = NULL;
    if(opd){
        sym = opd->getEntry();
        bool nconst = !sym->isConstant();    
        if(nconst){
            for(auto iter = ACP.begin(); iter!=ACP.end();iter++){
                SymbolEntry* key = iter->first;
                SymbolEntry* value = iter->second;
                if(sym == key){
                    flag = true;
                    return value;
                }
            }
        }
    }
    flag = false;
    return sym;
}

/* 工具函数: 求交集 */
//两个map求交集
USS map_intersection(USS u1, USS u2){
    USS res;
	for(auto i: u1){
        if(u2.find(i.first) != u2.end()){
            if(u2[i.first] == i.second){
                res[i.first] = i.second;
            }
        }
    }
    return res;
}

USS map_union(USS u1, USS u2){
    USS res;
    for (auto v : u1) {
        res[v.first] = v.second;
    }
    for (auto v : u2) {
        res[v.first] = v.second;
    }
    return res;
}

//多个map求交集
USS intersection(BasicBlock* bb){
    USS res;
    bb_iterator iter = bb->pred_begin();
    bb_iterator end = bb->pred_end();
    while(iter != end){
        int no = (*iter)->getNo();
        if(st[no]){
            res = map_intersection(res, CPout[no]);
        }
        iter++;
    }
    return res;
}

void CopyProp::copy_prop(){
    auto iter = unit->begin();
    USS ACP;
    while (iter != unit->end()){
        vector<BasicBlock*> block_list = (*iter)->getBlockList();
        BasicBlock* bb = (*iter)->getEntry(); // 函数入口基本块
        COPY[bb->getNo()];
        queue<BasicBlock*> q;
        q.push(bb);
        bool first = true;
        while(!q.empty()){
            BasicBlock* bb = q.front();
            q.pop();
            int no = bb->getNo();
            st[no] = true;
            if(first){
                CPin[no];
                first = false;
            }
            else{
                CPin[no] = intersection(bb);
            }
            local_copy_prop(bb, ACP);
            for(auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++){
                if(!st[(*succ)->getNo()]){
                    q.push(*succ);
                }
            }
        }
        iter++;
    }
    // global_copy_prop
    iter = unit->begin();
    ACP.clear();
    while (iter != unit->end()){
        vector<BasicBlock*> block_list = (*iter)->getBlockList();
        for(auto bb: block_list){
            ACP = CPin[bb->getNo()];
            local_copy_prop(bb, ACP);
        }
        iter++;
    }
}

void CopyProp::local_copy_prop(BasicBlock* bb, USS ACP){
    int no = bb->getNo();
    opds[no];
    load_sym[no];
    bool flag;
    for(auto iter = bb->begin();iter!=bb->end();iter=iter->getNext()){
        vector<Operand*> operands(iter->getOperands());
        if (iter->isBin())
        {
            if(!operands[1]->getEntry()->isConstant()){
                auto iter_def = operands[1]->getDef();
                if(iter_def){
                    vector<Operand*> def_operands(iter_def->getOperands());
                    SymbolEntry* sym1 = copy_value(def_operands[1], ACP, flag);
                    // 多余load指令删除
                    if(flag){
                        operands[1]->setEntry(load_sym[no][sym1]);
                        iter_def->remove();
                        opds[no][sym1]->addUse(iter);
                        operands[1]->setDef(opds[no][sym1]->getDef());
                    }
                }
            }
            if(!operands[2]->getEntry()->isConstant()){
                auto iter_def = operands[2]->getDef();
                if(iter_def){
                    vector<Operand*> def_operands(iter_def->getOperands());
                    SymbolEntry* sym2 = copy_value(def_operands[1], ACP, flag);
                    if(flag){
                        operands[2]->setEntry(load_sym[no][sym2]);
                        iter_def->remove();
                        opds[no][sym2]->addUse(iter);
                        operands[2]->setDef(opds[no][sym2]->getDef());
                    }
                }
            }
        }
        else if(iter->isStore()){
            if(!operands[1]->getEntry()->isConstant())
            {
                auto iter_def = operands[1]->getDef();
                if(iter_def && iter_def->isLoad()){
                    SymbolEntry* sym1 = operands[0]->getEntry();
                    remove_ACP(ACP, sym1);
                    remove_ACP(CPin[no], sym1); // cpin[i]-kill[i]
                    vector<Operand*> prev_operands(iter_def->getOperands());
                    SymbolEntry* sym2 = copy_value(prev_operands[1], ACP, flag);
                    if(flag){
                        operands[1]->setEntry(load_sym[no][sym2]);
                        iter_def->remove();
                        opds[no][sym2]->addUse(iter);
                        operands[1]->setDef(opds[no][sym2]->getDef());
                    } else{
                        opds[no][sym2] = prev_operands[0];
                        load_sym[no][sym2] = prev_operands[0]->getEntry();
                    }
                    ACP[sym1] = sym2;
                }
            }
        }
        operands.clear();
    }

    CPout[no] = map_union(ACP, CPin[no]);
    ACP.clear();
}


CopyProp::~CopyProp(){
    COPY.clear(), KILL.clear(), CPin.clear(), CPout.clear(), load_sym.clear(), opds.clear();
}