#include "SpecialOptimize.h"

using namespace std;

void SpecialOptimize::pass() {
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void SpecialOptimize::pass(Function* function) {
    // 处理循环对一维数组赋0值 bitset
    // while (i < n) {
    //     res[i] = 0;
    //     i = i + 1;
    // }
    vector<BasicBlock*> rmvList;
    for (auto block : function->getBlockList()) {
        auto in = block->begin();
        if (!in->isPhi())
            continue;
        auto uses = in->getUse();
        bool flag = false;
        auto phiDef = in->getDef();
        for (auto u : uses)
            if (u->isConst() && u->getConstVal() == 0) {
                flag = true;
                break;
            }
        if (!flag)
            continue;
        in = in->getNext();
        if (!in->isLoad())
            continue;
        auto loadDef = in->getDef();
        auto loadUse = in->getUse()[0];
        in = in->getNext();
        if (!in->isGep())
            continue;
        uses = in->getUse();
        if (uses[0] != loadDef || uses[1] != phiDef)
            continue;
        auto gepDef = in->getDef();
        in = in->getNext();
        if (!in->isStore())
            continue;
        uses = in->getUse();
        if (uses[0] != gepDef ||
            !(uses[1]->isConst() && uses[1]->getConstVal() == 0))
            continue;
        in = in->getNext();
        uses = in->getUse();
        if (!in->isAdd())
            continue;
        if (uses[0] != phiDef ||
            !(uses[1]->isConst() && uses[1]->getConstVal() == 1))
            continue;
        auto preds = block->getPred();
        if (preds.size() != 2)
            continue;
        auto pred = preds[0] == block ? preds[1] : preds[0];
        auto succs = block->getSucc();
        if (succs.size() != 2)
            continue;
        auto succ = succs[0] == block ? succs[1] : succs[0];
        rmvList.push_back(block);
        auto cond = pred->rbegin();
        auto cmp = cond->getPrev();
        pred->remove(cond);
        pred->remove(cmp);
        auto n = cmp->getUse()[1];
        auto shlDst = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        auto two = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 2));
        new ShlInstruction(shlDst, n, two, pred);
        auto loadDst = new Operand(new TemporarySymbolEntry(
            new PointerType(TypeSystem::intType), SymbolTable::getLabel()));
        new LoadInstruction(loadDst, loadUse, pred);
        auto int8PtrType = new PointerType(TypeSystem::int8Type);
        Operand* int8Ptr = new Operand(
            new TemporarySymbolEntry(int8PtrType, SymbolTable::getLabel()));
        auto bitcast = new BitcastInstruction(int8Ptr, loadDst, pred);
        bitcast->setFlag();
        std::string name = "llvm.memset.p0.i32";
        auto funcSE = identifiers->lookup(name);
        if (!funcSE) {
            vector<Type*> vec;
            vec.push_back(int8PtrType);
            vec.push_back(TypeSystem::int8Type);
            vec.push_back(TypeSystem::intType);
            vec.push_back(TypeSystem::boolType);
            vector<SymbolEntry*> vec1;
            auto funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
            SymbolTable* st = identifiers;
            while (st->getPrev())
                st = st->getPrev();
            funcSE = new IdentifierSymbolEntry(funcType, name, st->getLevel());
            st->install(name, funcSE);
            unit->insertDeclare(funcSE);
        }
        vector<Operand*> params;
        params.push_back(int8Ptr);
        params.push_back(
            new Operand(new ConstantSymbolEntry(TypeSystem::int8Type, 0)));
        params.push_back(shlDst);
        params.push_back(
            new Operand(new ConstantSymbolEntry(TypeSystem::boolType, 0)));
        new CallInstruction(nullptr, funcSE, params, pred);
        new UncondBrInstruction(succ, pred);
    }
    for (auto block : rmvList) {
        for (auto pred : block->getPred())
            pred->removeSucc(block);
        for (auto succ : block->getSucc())
            succ->removePred(block);
        function->remove(block);
    }
}