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
    iter = unit->begin();
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
        vector<Operand*> operands(iter->getOperands());
        if (iter->isBin())
        {
            int op = ((BinaryInstruction*)iter)->getOp();
            Instruction* p = iter;
            bool found = false;
            int len = AEB.size();
            int i = 0;
            for(; i<len; i++){
                if(op == AEB[i].opr && (operands[1] == AEB[i].opd1 || (operands[1]->isConst() && AEB[i].opd1->isConst() && operands[1]->getConstVal() == AEB[i].opd1->getConstVal()))
                && (operands[2] == AEB[i].opd2  || (operands[2]->isConst() && AEB[i].opd2->isConst() && operands[2]->getConstVal() == AEB[i].opd2->getConstVal()))){
                    found = true;
                    break;
                }
            }
            if(found){
                p = AEB[i].inst;
                Operand* dst = AEB[i].tmp;
                if(dst == nullptr){
                    dst = new Operand(new TemporarySymbolEntry(
                    operands[1]->getType(), SymbolTable::getLabel()));
                    vector<Operand*> pOperands(p->getOperands());
                    Instruction* inst = new BinaryInstruction(op, dst, pOperands[1], pOperands[2], nullptr);
                    AEB[i].tmp = dst;
                    bb->insertBefore(inst, p);
                    Instruction* inst1 = new BinaryInstruction(BinaryInstruction::ADD, pOperands[0], dst, new Operand(new ConstantSymbolEntry(dst->getType(), 0)), nullptr);
                    bb->insertBefore(inst1, p);
                    bb->remove(p);
                }
                Instruction* inst2 = new BinaryInstruction(BinaryInstruction::ADD, operands[0], dst, new Operand(new ConstantSymbolEntry(dst->getType(), 0)), nullptr);
                bb->insertBefore(inst2, iter);
                bb->remove(iter);
            } else {
                // insert
                struct aeb tmp;
                tmp.inst = (BinaryInstruction*)iter, tmp.opd1 = operands[1], tmp.opr = op, tmp.opd2 = operands[2];
                AEB.push_back(tmp);
                delAEB(operands[0], AEB);
            }
        }
    }
}

ElimComSubexpr::~ElimComSubexpr(){
    AEB.clear();
}
