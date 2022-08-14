#include "Operand.h"
#include <string.h>
#include <algorithm>
#include <sstream>
#include <typeinfo>

std::string Operand::toStr() const {
    std::string res = se->toStr(); // llvm ir global variable with `@0`
    if (typeid(*se) == typeid(IdentifierSymbolEntry)) {
        if (((IdentifierSymbolEntry*)se)->isGlobal()) {
            res = "@" + res;
        }
    }
    return res;
}

bool Operand::isSSAName() {
    if(se->isTemporary()){
        return true;
    }
    else if(se->isVariable() || this->isGlobal()){
        if(se->getType()->isArray()){
            return false;
        }
        return true;
    }
    return false; // constant
}

bool Operand::isInArray() {
    // if(this->getDef()->isGep()) return true;
    return false;
}

std::pair<int, int> Operand::getInitLatticeValue(){
    if(this->isConst()){
        return {0, ((ConstantSymbolEntry*)se)->getValue()};
    }
    else if(se->isTemporary()){
        if(this->isParam())
            return {-1, 0};
        return {1, 0};
    }
    else if(se->isVariable()){
        if(((IdentifierSymbolEntry*)se)->isConstant()){
            return {0, ((IdentifierSymbolEntry*)se)->getValue()};
        }
        // else if(this->isInArray()){
        //     double* arrayValue = ((GepInstruction*)this->getDef())->getArrayValue();
        //     int idx = ((GepInstruction*)this->getDef())->getFlatIdx();
        //     if(arrayValue != nullptr && idx != -1 && idx <= sizeof(arrayValue)/sizeof(arrayValue[0])){
        //         return {0, arrayValue[idx]};
        //     }
        // }
    }
    return {-1, 0};
}

void Operand::removeUse(Instruction* inst) {
    auto i = std::find(uses.begin(), uses.end(), inst);
    if (i != uses.end())
        uses.erase(i);
}

void Operand::removeDef(Instruction* inst) {
    def = nullptr;
}

Operand::Operand(const Operand& o) {
    if (o.se->isTemporary()) {
        se = new TemporarySymbolEntry(o.se->getType(), SymbolTable::getLabel());
    }
}