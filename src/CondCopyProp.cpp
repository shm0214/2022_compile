#include "CondCopyProp.h"
#include "unit.h"
using namespace std;

void CondCopyProp::pass()
{
    for(auto func = unit->begin(); func != unit->end(); func++)
        constantPropagation(*func);
}

void CondCopyProp::constantPropagation(Function *func)
{
    cout<<"func"<<endl;
    std::map<Operand*, std::pair<char, int>> value;
    std::vector<Operand*> worklist;
    std::vector<BasicBlock*> block_list = func->getBlockList();
    for(auto bb : block_list){
        for (auto inst = bb->begin(); inst != bb->end(); inst = inst->getNext())
        {
            Operand *def = inst->getDef();
            if (def == nullptr)
                continue;
            if (def->isSSAName())
            {
                value[def] = inst->getLatticeValue(value);
                if (value[def].first != 1)
                    worklist.push_back(def);
            }
        }
    }
    cout<<"test1"<<endl;
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
                std::pair<char, int> old_val = value[m];
                value[m] = inst->getLatticeValue(value);
                if (value[m] != old_val)
                    worklist.push_back(m);
            }
        }
    }
    cout<<"test2"<<endl;
    std::vector<Instruction *> delete_list;
    for (auto &op : value)
    {
        if (op.second.first == -1)
            continue;
        Operand *cst;
        // cst = unit->getConstant(op.second.second);
        cst = new Operand(new ConstantSymbolEntry(op.first->getType(), op.second.second));
        Instruction *def = op.first->getDef();
        std::vector<Instruction *> use = op.first->getUse();
        delete_list.push_back(def);
        for (auto &use_inst : use)
            use_inst->replaceUse(op.first, cst);
    }
    cout<<"test3"<<endl;
    for (auto &i : delete_list)
        i->getParent()->remove(i);
    cout<<"end"<<endl;
}

CondCopyProp::~CondCopyProp(){

}