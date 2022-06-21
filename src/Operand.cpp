#include "Operand.h"
#include <string.h>
#include <algorithm>
#include <sstream>

std::string Operand::toStr() const {
    return se->toStr();
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