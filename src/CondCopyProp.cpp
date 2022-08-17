#include "CondCopyProp.h"
using namespace std;

void CondCopyProp::pass()
{
    for(auto func = unit->begin(); func != unit->end(); func++)
        constantPropagation(*func);
}

void CondCopyProp::constantPropagation(Function *func)
{
    std::map<Operand*, std::pair<int, int>> value;
    std::vector<Operand*> worklist;

    std::vector<BasicBlock*> block_list = func->getBlockList();
    for(auto bb : block_list){
        for (auto inst = bb->begin(); inst != bb->end(); inst = inst->getNext())
        {
            Operand *def = inst->getDef();
            if (def == nullptr){
                continue;
            }
            if(!inst->isGep()){
                std::vector<Operand*> uses(inst->getUse());
                for(auto src : uses){
                    if(!src->isConst() && src->isConArray()){                        
                        double* arrayValue = ((GepInstruction*)src->getDef())->getArrayValue();
                        int idx = ((GepInstruction*)src->getDef())->getFlatIdx();
                        if(arrayValue != nullptr && idx != -1){
                            value[src] =  {0, arrayValue[idx]};
                            inst->replaceUse(src, new Operand(new ConstantSymbolEntry(def->getType(), arrayValue[idx])));
                            if(!src->usersNum()){
                                GepInstruction* i = (GepInstruction*)src->getDef();
                                while(true)
                                {
                                    i->getParent()->remove(i);
                                    if(!i->getFirst())
                                        i = (GepInstruction*)i->getUse()[0]->getDef();
                                    else
                                        break;
                                } 
                            }
                        }
                    }
                }
            }            
            if (def->isSSAName())
            {
                value[def] = inst->getLatticeValue(value);
                if (value[def].first != 1){
                    worklist.push_back(def);
                    // cout<<"def:"<<def->getEntry()->toStr()<<" , value: "<<value[def].first<<", "<<value[def].second<<endl;
                }                    
            }
        }
    }

    while (!worklist.empty())
    {
        Operand *d = worklist.back();
        worklist.pop_back();
        for (auto &inst : d->getUse())
        {
            Operand *m = inst->getDef();
            if (m == nullptr || value.find(m) == value.end())
                continue;
            if (value[m].first != -1)
            {
                std::pair<int, int> old_val = value[m];
                value[m] = inst->getLatticeValue(value);
                if (value[m] != old_val){
                    worklist.push_back(m);
                    // cout<<"m:"<<m->getEntry()->toStr()<<" , value: "<<value[m].first<<", "<<value[m].second<<endl;
                }                 
            }
        }
    }
    std::vector<Instruction *> delete_list;
    for (auto &op : value)
    {
        if (op.second.first == -1 || op.second.first == 1)
            continue;
        Operand *cst;
        cst = new Operand(new ConstantSymbolEntry(op.first->getType(), op.second.second));
        Instruction *def = op.first->getDef();
        if(!def->isCmp()){
            std::vector<Instruction *> use = op.first->getUse();
            delete_list.push_back(def);
            for (auto &use_inst : use){
                use_inst->replaceUse(op.first, cst);
                // if(use_inst->isCond()){
                //     BasicBlock* bb = use_inst->getParent();
                //     BasicBlock* true_branch = ((CondBrInstruction*)use_inst)->getTrueBranch();
                //     BasicBlock* false_branch = ((CondBrInstruction*)use_inst)->getFalseBranch();
                //     if(op.second.second){
                //         Instruction* new_ = new UncondBrInstruction(true_branch, nullptr);
                //         bb->replaceIns(use_inst, new_);
                //         bb->removeSucc(false_branch);
                //         false_branch->removePred(bb);
                //         bb->getParent()->remove(false_branch);
                //     }
                //     else{
                //         Instruction* new_ = new UncondBrInstruction(false_branch, nullptr);
                //         bb->replaceIns(use_inst, new_);
                //         bb->removeSucc(true_branch);
                //         true_branch->removePred(bb);
                //         bb->getParent()->remove(true_branch);
                //     }
                // }
            }     
        }       
    }
    for (auto &i : delete_list){
        // cout<<"del"<<endl;
        i->getParent()->remove(i);
    }        
}

CondCopyProp::~CondCopyProp(){

}