#include "Div2Mul.h"
#include <cmath>
using LL = long long;

const int N = 32;
inline int clz(int x) { return __builtin_clz(x); }
inline int ctz(int x) { return __builtin_ctz(x); }

struct Multiplier {
    LL m;
    int l;
};

Multiplier chooseMultiplier(int d, int p) {
    assert(d != 0);
    assert(p >= 1 && p <= N);
    int l = floor(log2(d));
    LL high = ceil((double(LL(1) << (l+32)) / d));
    return {high, l};
}

void Div2Mul::pass()
{
    for(auto func = unit->begin(); func != unit->end(); func++)
        div2mul(*func);
}

void Div2Mul::div2mul(MachineFunction* func){
    for(auto bb :  func->getBlocks()){
        if(bb && bb->getSize()){
            std::vector<MachineInstruction*> insts;
            insts.assign(bb->begin(), bb->end());
            for(auto inst : insts){
                if(inst->isDivConst()){
                    int d = inst->getUse()[1]->getDef()->getUse()[0]->getVal();
                    int s = ctz(d);
                    auto off1 = new MachineOperand(MachineOperand::IMM, 31);    
                    auto tmp = new MachineOperand(
                                    MachineOperand::VREG,
                                    SymbolTable::getLabel());                    
                    if(d == (int(1) << s)){
                        // d = 2**n
                        MachineInstruction* inst1 = new MovMInstruction(nullptr, MovMInstruction::MOVLSR,
                                                    tmp, inst->getUse()[0], MachineInstruction::NONE, off1);
                        bb->insertBefore(inst1, inst);
                        auto off = new MachineOperand(MachineOperand::IMM, s);                        
                        MachineInstruction* inst2 = new MovMInstruction(nullptr, MovMInstruction::MOVASR,
                                                inst->getDef()[0], inst->getUse()[0], MachineInstruction::NONE, off);                   
                        bb->insertBefore(inst2, inst);
                        
                        MachineInstruction* inst5 = new BinaryMInstruction(nullptr, BinaryMInstruction::ADD,
                                                inst->getDef()[0], inst->getDef()[0], tmp);
                        bb->insertBefore(inst5, inst);
                        bb->remove(inst);
                    }
                    else {
                        int a = 0;
                        if(d % (int(1) << s) == 0){
                            d = d / (int(1) << s);
                            a = s;
                        }
                        Multiplier multi = chooseMultiplier(d, N);
                        // printf("m:%lld, l:%d\n", multi.m, multi.l+a);
                        if(multi.m < (LL(1) << (N-1))){
                            auto m = new MachineOperand(MachineOperand::IMM, multi.m);
                            auto l = new MachineOperand(MachineOperand::IMM, multi.l+a);
                            auto rh = new MachineOperand(
                                MachineOperand::VREG,
                                SymbolTable::getLabel());
                            auto rl = new MachineOperand(
                                MachineOperand::VREG,
                                SymbolTable::getLabel());
                            MachineInstruction* inst1 = new LoadMInstruction(nullptr, LoadMInstruction::LDR, tmp, m);
                            MachineInstruction* inst2 = new SmullMInstruction(nullptr, rl, rh, inst->getUse()[0], tmp);
                            MachineInstruction* inst3 = new MovMInstruction(nullptr, MovMInstruction::MOVASR,
                                                inst->getDef()[0], rh, MachineInstruction::NONE, l);
                            MachineInstruction* inst4 = new MovMInstruction(nullptr, MovMInstruction::MOVLSR,
                                                tmp, rh, MachineInstruction::NONE, off1);
                            MachineInstruction* inst5 = new BinaryMInstruction(nullptr, BinaryMInstruction::ADD,
                                                inst->getDef()[0], inst->getDef()[0], tmp);
                        
                            bb->insertBefore(inst1, inst);
                            bb->insertBefore(inst2, inst);
                            bb->insertBefore(inst3, inst);
                            bb->insertBefore(inst4, inst);
                            bb->insertBefore(inst5, inst);
                            bb->remove(inst);
                        }
                    }
                }
                else if(inst->isMod()){
                    int d = inst->getUse()[1]->getDef()->getUse()[0]->getVal();
                    if((d > 0) && ((d & (d-1)) == 0)){
                        auto off = new MachineOperand(MachineOperand::IMM, d-1);
                        auto off1 = new MachineOperand(MachineOperand::IMM, 31);
                        auto tmp = new MachineOperand(
                                    MachineOperand::VREG,
                                    SymbolTable::getLabel());

                        MachineInstruction* inst1 = new BinaryMInstruction(nullptr, BinaryMInstruction::AND,
                                                inst->getDef()[0], inst->getUse()[0], off);
                        MachineInstruction* inst2 = new MovMInstruction(nullptr, MovMInstruction::MOVLSR,
                                                tmp, inst->getUse()[0], MachineInstruction::NONE, off1);
                        MachineInstruction* inst3 = new BinaryMInstruction(nullptr, BinaryMInstruction::ADD,
                                                inst->getDef()[0], inst->getDef()[0], tmp);
                        bb->insertBefore(inst1, inst);
                        bb->insertBefore(inst2, inst);
                        bb->insertBefore(inst3, inst);
                        bb->remove(inst);
                    }
                }
            }
        }
    }
}

Div2Mul::~Div2Mul(){
    
}