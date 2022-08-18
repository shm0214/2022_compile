#include "Div2Mul.h"
const int N = 32;

inline int clz(uint32_t x) {
    return __builtin_ctz(x);
}

struct Multiplier {
    uint64_t m;
    int l;
};

Multiplier chooseMultiplier(uint32_t d) {
    assert(d!=0);
    int l = N - clz(d-1);
    uint64_t low = (uint64_t(1) << (N+1)) / d;
    uint64_t high = (uint64_t(1) << (N+1)) + (uint64_t(1)<<l) / d;
    while((low >> 1) < (high >> 1) && l > 0){
        low >>= 1, high >>= 1, --l;
    }
    return {high, l};
}

void Div2Mul::pass()
{
    for(auto func = unit->begin(); func != unit->end(); func++)
        div2mul(*func);
}

void Div2Mul::div2mul(Function* func){
    std::vector<BasicBlock*> block_list = func->getBlockList();
    for(auto bb : block_list){
        for(auto inst = bb->begin(); inst != bb->end(); inst = inst->getNext()){
            if(inst->isDivConst()){
                uint32_t divisor = int(inst->getUse()[1]->getConstVal());
                Multiplier res = chooseMultiplier(divisor);
                Operand* opd1 = new Operand(new ConstantSymbolEntry(inst->getDef()->getType, res.m));
                Operand* opd2 = new Operand(new ConstantSymbolEntry(inst->getDef()->getType, res.l));
                Operand* dst = new Operand(new TemporarySymbolEntry(
                inst->getDef()->getType(), SymbolTable::getLabel()));

                Instruction* new1 = new BinaryInstruction(BinaryInstruction::MUL, dst, inst->getUse()[0], opd1, nullptr)
                Instruction* new2 = new BinaryInstruction(BinaryInstruction::ASHR, inst->getDef(), dst, opd2, nullptr)
                inst->getParent()->insertBefore(new1, inst);
                inst->getParent()->insertBefore(new2, inst);
                inst->getParent()->remove(inst);
            }
        }
    }
}