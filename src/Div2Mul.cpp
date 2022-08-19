#include "Div2Mul.h"
#include <cmath>

using Uint32 = unsigned int;
using Uint64 = unsigned long long;
using Int32 = int;
using Int64 = long long;

using namespace std;

const int N = 32;
inline int clz(Uint32 x) { return __builtin_clz(x); }
inline int ctz(Uint32 x) { return __builtin_ctz(x); }

Uint32 muluh(Uint32 a, Uint32 b) { return (Uint64(a) * b) >> 32; }
int mulsh(int a, int b) { return (Int64(a) * b) >> 32; }

struct Multiplier {
    Uint64 m;
    int l;
};

Multiplier chooseMultiplier(Uint32 d, int p) {
    assert(d != 0);
    assert(p >= 1 && p <= N);
    // l = ceil(log2(d))
    int l = N - clz(d - 1);
    Uint64 low = (Uint64(1) << (N + l)) / d;
    Uint64 high = ((Uint64(1) << (N + l)) + (Uint64(1) << (N + l - p))) / d;
    while((low >> 1) < (high >> 1) && l > 0)
        low >>= 1, high >>= 1, --l;
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
                    printf("isdivconst\n");
                    int d = inst->getUse()[1]->getDef()->getUse()[0]->getVal();
                    int s = ctz(d);
                    if(d == (Uint32(1) << s)){
                        // d = 2**n
                        auto off = new MachineOperand(MachineOperand::IMM, s);
                        auto off1 = new MachineOperand(MachineOperand::IMM, 31);
                        auto off2 = new MachineOperand(MachineOperand::IMM, 30);
                        auto tmp = new MachineOperand(
                                    MachineOperand::VREG,
                                    SymbolTable::getLabel());

                        MachineInstruction* inst1 = new MovMInstruction(nullptr, MovMInstruction::MOVASR,
                                                tmp, inst->getUse()[0], MachineInstruction::NONE, off1);
                        MachineInstruction* inst2 = new MovMInstruction(nullptr, MovMInstruction::MOVLSR,
                                                tmp, tmp, MachineInstruction::NONE, off2);
                        MachineInstruction* inst3 = new BinaryMInstruction(nullptr, BinaryMInstruction::ADD,
                                                inst->getDef()[0], inst->getUse()[0], tmp);
                        MachineInstruction* inst4 = new MovMInstruction(nullptr, MovMInstruction::MOVASR,
                                                inst->getDef()[0], inst->getDef()[0], MachineInstruction::NONE, off);
                                                
                        bb->insertBefore(inst4, inst);
                        bb->insertBefore(inst3, inst4);
                        bb->insertBefore(inst2, inst3);
                        bb->insertBefore(inst1, inst2);
                        bb->remove(inst);
                    }
                    else if(d < (Uint32(1) << (N - 1))){
                        Multiplier multi = chooseMultiplier(d, N);
                        if(multi.m < (Uint64(1) << N)) s = 0;
                        else multi = chooseMultiplier(d >> s, N - s);
                        if(multi.m < (Uint64(1) << N)){
                            // muluh(n >> s, multi.m) >> multi.l
                            auto off = new MachineOperand(MachineOperand::IMM, s);
                            auto off1 = new MachineOperand(MachineOperand::IMM, 32);
                            auto m = new MachineOperand(MachineOperand::IMM, int(multi.m));
                            auto l = new MachineOperand(MachineOperand::IMM, multi.l);
                            if(s > 0){
                                MachineInstruction* inst1 = new MovMInstruction(nullptr, MovMInstruction::MOVASR,
                                                inst->getDef()[0], inst->getUse()[0], MachineInstruction::NONE, off);
                                bb->insertBefore(inst1, inst);
                            }
                            MachineInstruction* inst2 = new BinaryMInstruction(nullptr, BinaryMInstruction::MUL,
                                                inst->getDef()[0], inst->getDef()[0], m);
                            MachineInstruction* inst3 = new MovMInstruction(nullptr, MovMInstruction::MOVASR,
                                                inst->getDef()[0], inst->getDef()[0], MachineInstruction::NONE, off1);
                            bb->insertBefore(inst2, inst);
                            bb->insertBefore(inst3, inst);
                            if(multi.l > 0){
                                MachineInstruction* inst4 = new MovMInstruction(nullptr, MovMInstruction::MOVASR,
                                                inst->getDef()[0], inst->getDef()[0], MachineInstruction::NONE, l);
                                bb->insertBefore(inst4, inst);
                            }                            
                            bb->remove(inst);
                        }
                    }
                }
                else if(inst->isModConst()){
                    int d = inst->getUse()[1]->getDef()->getUse()[0]->getVal();
                    if((d > 0) && ((d & (d-1)) == 0)){
                        auto off = new MachineOperand(MachineOperand::IMM, d-1);
                        auto off1 = new MachineOperand(MachineOperand::IMM, 31);
                        auto off2 = new MachineOperand(MachineOperand::IMM, 30);
                        auto tmp = new MachineOperand(
                                    MachineOperand::VREG,
                                    SymbolTable::getLabel());

                        MachineInstruction* inst1 = new MovMInstruction(nullptr, MovMInstruction::MOVASR,
                                                tmp, inst->getUse()[0], MachineInstruction::NONE, off1);
                        MachineInstruction* inst2 = new MovMInstruction(nullptr, MovMInstruction::MOVLSR,
                                                tmp, tmp, MachineInstruction::NONE, off2);
                        MachineInstruction* inst3 = new BinaryMInstruction(nullptr, BinaryMInstruction::ADD,
                                                inst->getDef()[0], inst->getUse()[0], tmp);
                        MachineInstruction* inst4 = new BinaryMInstruction(nullptr, BinaryMInstruction::AND,
                                                inst->getDef()[0], inst->getDef()[0], off);
                        bb->insertBefore(inst4, inst);
                        bb->insertBefore(inst3, inst4);
                        bb->insertBefore(inst2, inst3);
                        bb->insertBefore(inst1, inst2);
                        bb->remove(inst);
                    }
                }
            }
        }
    }
}

Div2Mul::~Div2Mul(){

}