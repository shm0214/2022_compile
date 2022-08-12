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
    else if(se->isVariable()){
        if(se->getType()->isArray() || this->isGlobal()){
            return false;
        }
        return true;
    }
    return false; // constant
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