#ifndef __OPERAND_H__
#define __OPERAND_H__

#include <vector>
#include "SymbolTable.h"
#include "Type.h"

class Instruction;
class Function;

// class Operand - The operand of an instruction.
class Operand {
    typedef std::vector<Instruction*>::iterator use_iterator;

   private:
    Instruction* def;  // The instruction where this operand is defined.
    std::vector<Instruction*> uses;  // Intructions that use this operand.
    SymbolEntry* se;                 // The symbol entry of this operand.
   public:
    Operand(SymbolEntry* se) : se(se) { def = nullptr; };
    Operand(const Operand& o);
    void setDef(Instruction* inst) { def = inst; };
    void addUse(Instruction* inst) { uses.push_back(inst); };
    void removeUse(Instruction* inst);
    void removeDef(Instruction* inst);
    int usersNum() const { return uses.size(); };
    bool isSSAName();
    use_iterator use_begin() { return uses.begin(); };
    use_iterator use_end() { return uses.end(); };
    Type* getType() { return se->getType(); };
    std::string toStr() const;
    SymbolEntry* getEntry() { return se; };
    void setEntry(SymbolEntry* se) { this->se = se; };
    Instruction* getDef() { return def; };
    std::vector<Instruction*> getUse() { return uses; };
    std::pair<int, int> getInitLatticeValue();
    bool isZero() const {
        if (se->isConstant()) {
            ConstantSymbolEntry* cse = (ConstantSymbolEntry*)se;
            if (cse->getValue() == 0)
                return true;
        }
        return false;
    }
    bool isConst() const { return se->isConstant(); }
    double getConstVal() const {
        return ((ConstantSymbolEntry*)se)->getValue();
    }
    bool isConArray();
    int getLabel() const { return se->getLabel(); }
    bool isParam() const {
        if (se->isVariable())
            return ((IdentifierSymbolEntry*)se)->isParam();
        return false;
    }
    // used for auto inline
    int getParamNo() const {
        return ((IdentifierSymbolEntry*)se)->getAllParamNo();
    }
    bool isGlobal() const {
        if (se->isVariable())
            return ((IdentifierSymbolEntry*)se)->isGlobal();
        return false;
    }
};

#endif