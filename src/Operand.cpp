#include "Operand.h"
#include <string.h>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include "Instruction.h"

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

bool Operand::isConArray() {
    if(this->getDef() && this->getDef()->isGep()) {
        return ((ArrayType*)(((PointerType*)(this->getDef()->getUse()[0]->getEntry()->getType()))->getType()))->isConst();
    }
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
        if(this->isGlobal()){
            PointerType* type = (PointerType*)(this->getType());
            if(type->getType()->isInt() && ((IntType*)type->getType())->isConst()){
                return {0, ((IdentifierSymbolEntry*)se)->getValue()};
            }
            else if(type->getType()->isFloat() && ((FloatType*)type->getType())->isConst()){
                return {0, ((IdentifierSymbolEntry*)se)->getValue()};
            }          
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