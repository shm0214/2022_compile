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
