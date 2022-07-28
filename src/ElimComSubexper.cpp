#include "ElimComSubexpr.h"
#include <set>
#include <algorithm>
#include <queue>
typedef vector<BasicBlock*>::iterator bb_iterator;
typedef vector<struct aeb> VAEB;
using namespace std;
const int N = 2000100;
bool ste[N]; // 标记基本块是否已遍历过
unordered_map<int, VAEB> EVAL, AEin, AEout;


void delAEB(SymbolEntry* sym, VAEB AEB){
    int len = AEB.size();
    for(int i=0;i<len;i++){
        if(sym == AEB[i].opd1 || sym  == AEB[i].opd2){
            for(int j=i; j<len-1; j++){
                AEB[j] = AEB[j+1];
            }
            len--;
        }
    }
}

//两个vector求交集
VAEB vec_intersection(VAEB v1,VAEB v2){
    VAEB v11, v22, res;
    int len = min(v1.size(), v2.size());
    if(v1.size()>v2.size()){
        v11 = v2;
        v22 = v1;
    }else{
        v11 = v1;
        v22 = v2;
    }
    VAEB::iterator it;
    for(int i=0; i<len;i++){
        it = find(v22.begin(), v22.end(), v11[i]);
        if (it != v22.end()) {
            res.push_back(v11[i]);
        }
    }
    return res;
}
 
//两个vector求并集
VAEB vec_union(VAEB v1,VAEB v2){
    VAEB res(v1);
    VAEB::iterator it;
    for(unsigned i=0;i<v2.size();i++){
        it = find(v1.begin(), v1.end(), v2[i]);
        if (it == v1.end()) {
            res.push_back(v2[i]);
        }
    }
    return res;
}

//多个map求交集
VAEB vintersection(BasicBlock* bb){
    VAEB res;
    bb_iterator iter = bb->pred_begin();
    bb_iterator end = bb->pred_end();
    while(iter != end){
        int no = (*iter)->getNo();
        if(ste[no]){
            res = vec_intersection(res, AEout[no]);
        }
        iter++;
    }
    return res;
}

void ElimComSubexpr::elim_cse(){
    auto iter = unit->begin();
    VAEB AEB;
    while (iter != unit->end()){
        vector<BasicBlock*> block_list = (*iter)->getBlockList();
        BasicBlock* bb = (*iter)->getEntry(); // 函数入口基本块
        EVAL[bb->getNo()];
        queue<BasicBlock*> q;
        q.push(bb);
        bool first = true;
        while(!q.empty()){
            BasicBlock* bb = q.front();
            q.pop();
            int no = bb->getNo();
            ste[no] = true;
            if(first){
                AEin[no];
                first = false;
            }
            else{
                AEin[no] = vintersection(bb);
            }
            cout<<"local"<<endl;
            local_elim_cse(bb, AEB);
            for(auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++){
                if(!ste[(*succ)->getNo()]){
                    q.push(*succ);
                }
            }
        }
        iter++;
    }
    // global
    cout<<"global"<<endl;
    iter = unit->begin();
    AEB.clear();
    while (iter != unit->end()){
        vector<BasicBlock*> block_list = (*iter)->getBlockList();
        for(auto bb: block_list){
            AEB = AEin[bb->getNo()];
            local_elim_cse(bb, AEB);
        }
        iter++;
    }
}

void ElimComSubexpr::local_elim_cse(BasicBlock* bb, VAEB AEB){
    int no = bb->getNo();
    for(auto iter = bb->begin();iter!=bb->end();iter=iter->getNext()){
        vector<Operand*> operands(iter->getOperands());
        // if(iter->isLoad()) cout<<"load "<<operands[0]->getEntry()->toStr()<<", "<<operands[1]->getEntry()->toStr()<<endl; 
        if (iter->isBin())
        {
            cout<<"bin"<<endl;
            int op = ((BinaryInstruction*)iter)->getOp();
            Instruction* p = iter;
            bool found = false;

            SymbolEntry *sym1 = operands[1]->getEntry(), *sym2 = operands[2]->getEntry(); 
            if(((IdentifierSymbolEntry*)sym1)->isSysy() || ((IdentifierSymbolEntry*)sym2)->isSysy()) continue;
            if(!sym1->isConstant()){
                auto iter_def = operands[1]->getDef();
                if(iter_def){
                    vector<Operand*> def_operands(iter_def->getOperands());
                    sym1 = def_operands[1]->getEntry();
                }
            }
            cout<<"h2";
            if(!sym2->isConstant()){
                auto iter_def = operands[2]->getDef();
                if(iter_def){
                    vector<Operand*> def_operands(iter_def->getOperands());
                    sym2 = def_operands[1]->getEntry();
                }
            }
            cout<<"t2"<<endl;
            int len = AEB.size();
            // cout<<"len:"<<len<<", op:"<<op<<", opd1:"<<sym1->toStr()<<", opd2:"<<sym2->toStr()<<endl;
            // cout<<"len:"<<len<<", op:"<<op<<", opd0:"<<operands[0]->getEntry()->toStr()<<", opd1:"<<operands[1]->getEntry()->toStr()<<", opd2:"<<operands[2]->getEntry()->toStr()<<endl;
            int i = 0;
            for(; i<len; i++){
                if(op == AEB[i].opr && sym1->toStr() == AEB[i].opd1->toStr() && sym2->toStr() == AEB[i].opd2->toStr()){
                    found = true;
                    break;
                }
            }
            cout<<"t3"<<endl;
            if(found){
                cout<<"found"<<endl;
                p = AEB[i].inst;
                Operand* dst = AEB[i].tmp;
                if(dst == nullptr){
                    dst = new Operand(new TemporarySymbolEntry(
                    TypeSystem::intType, SymbolTable::getLabel()));
                    vector<Operand*> pOperands(p->getOperands());
                    Instruction* inst = new BinaryInstruction(op, dst, pOperands[1], pOperands[2], nullptr);
                    // cout<<"add a binInst "<<dst->getEntry()->toStr()<<", "<<pOperands[1]->getEntry()->toStr()<<", "<<pOperands[2]->getEntry()->toStr()<<endl;
                    AEB[i].tmp = dst;
                    bb->insertBefore(inst, p);
                    Instruction* inst1 = new LoadInstruction(pOperands[0], dst, nullptr);
                    // cout<<"revise p to load "<<pOperands[0]->getEntry()->toStr()<<", "<<dst->getEntry()->toStr()<<endl; 
                    bb->insertBefore(inst1, p);
                    bb->remove(p);
                }
                Instruction* inst2 = new LoadInstruction(operands[0], dst, nullptr);
                // cout<<"revise iter to load "<<operands[0]->getEntry()->toStr()<<", "<<dst->getEntry()->toStr()<<endl; 
                bb->insertBefore(inst2, iter);
                bb->remove(iter);
            } else {
                // insert
                cout<<"not found"<<endl;
                struct aeb tmp;
                tmp.inst = (BinaryInstruction*)iter, tmp.opd1 = sym1, tmp.opr = op, tmp.opd2 = sym2;
                AEB.push_back(tmp);
                SymbolEntry *sym = operands[0]->getEntry();
                auto iter_def = operands[0]->getDef();
                if(iter_def){
                    vector<Operand*> def_operands(iter_def->getOperands());
                    sym = def_operands[1]->getEntry();
                }
                delAEB(sym, AEB);
                delAEB(sym, AEin[no]);
            }
            cout<<"aeb size:"<< AEB.size() << "  bin end"<<endl;
        }
    }
    cout<<"this block end"<<endl;
    AEout[no] = vec_union(AEB, AEin[no]);
    AEB.clear();
}

ElimComSubexpr::~ElimComSubexpr(){
    AEin.clear(), AEout.clear();
}