#include "ElimComSubexpr.h"
typedef vector<struct aeb> VAEB;
using namespace std;
VAEB AEB;

void delAEB(Operand* opd, VAEB AEB){
    int len = AEB.size();
    for(int i=0;i<len;i++){
        if(opd == AEB[i].opd1 || opd  == AEB[i].opd2){
            for(int j=i; j<len-1; j++){
                AEB[j] = AEB[j+1];
            }
            len--;
        }
    }
}

void ElimComSubexpr::pass(){
    auto iter = unit->begin();
    while (iter != unit->end()){
        vector<BasicBlock*> block_list = (*iter)->getBlockList();
        for(auto bb: block_list){
            local_elim_cse(bb);
        }
        iter++;
    }
}

void ElimComSubexpr::local_elim_cse(BasicBlock* bb){
    for(auto iter = bb->begin();iter!=bb->end();iter=iter->getNext()){
        Operand* def = iter->getDef();
        vector<Operand*> uses(iter->getUse());
        if(uses.size() == 2){
            struct aeb t;
            t.inst = iter, t.opd1 = uses[0], t.opr = iter->getOpcode(), t.opd2 = uses[1];
            bool found = false;
            int i = 0;
            for(; i<int(AEB.size()); i++){
                if(t == AEB[i]){
                    found = true;
                    break;
                }
            }

            if(found){
                Instruction* p = AEB[i].inst;
                Operand* dst = AEB[i].tmp;
                if(dst == nullptr){
                    dst = new Operand(new TemporarySymbolEntry(
                    def->getType(), SymbolTable::getLabel()));
                    Instruction* inst = new BinaryInstruction(AEB[i].opr, dst, AEB[i].opd1, AEB[i].opd2, nullptr);
                    AEB[i].tmp = dst;
                    bb->insertBefore(inst, p);
                    Instruction* inst1 = new StoreInstruction(p->getDef(), dst, nullptr);
                    bb->insertBefore(inst1, p);
                    bb->remove(p);
                }
                Instruction* inst2 = new StoreInstruction(def, dst, nullptr);
                bb->insertBefore(inst2, iter);
                bb->remove(iter);
            } else {
                // insert
                AEB.push_back(t);
                delAEB(def, AEB);
            }
        }
    }
}

ElimComSubexpr::~ElimComSubexpr(){
    AEB.clear();
}
