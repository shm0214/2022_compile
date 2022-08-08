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