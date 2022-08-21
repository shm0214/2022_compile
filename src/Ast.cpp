#include "Ast.h"
#include <stack>
#include <string>
#include "IRBuilder.h"
#include "Instruction.h"
#include "SymbolTable.h"
#include "Type.h"
#include "Unit.h"
extern Unit unit;
extern MachineUnit mUnit;

#include <iostream>

extern FILE* yyout;
int Node::counter = 0;
IRBuilder* Node::builder;

Node::Node() {
    seq = counter++;
    next = nullptr;
}

void Node::setNext(Node* node) {
    Node* n = this;
    while (n->getNext()) {
        n = n->getNext();
    }
    if (n == this) {
        this->next = node;
    } else {
        n->setNext(node);
    }
}

void Node::backPatch(std::vector<Instruction*>& list, BasicBlock* bb) {
    for (auto& inst : list) {
        if (inst->isCond())
            dynamic_cast<CondBrInstruction*>(inst)->setTrueBranch(bb);
        else if (inst->isUncond())
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
    }
}

std::vector<Instruction*> Node::merge(std::vector<Instruction*>& list1,
                                      std::vector<Instruction*>& list2) {
    std::vector<Instruction*> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit* unit) {
    IRBuilder* builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void FunctionDef::genCode() {
    Unit* unit = builder->getUnit();
    Function* func = new Function(unit, se);
    BasicBlock* entry = func->getEntry();
    // set the insert point to the entry basicblock of this function.

    if (func->getSymPtr()->toStr() == "@fib") {
        builder->setInsertBB(entry);
        auto type = (FunctionType*)(func->getSymPtr()->getType());
        auto cmpDst = new Operand(new TemporarySymbolEntry(
            TypeSystem::boolType, SymbolTable::getLabel()));
        auto two = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 2));
        auto one = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 1));
        auto zero = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
        auto param = new Operand(type->getParamsSe()[0]);
        auto newParam = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        new BinaryInstruction(BinaryInstruction::ADD, newParam, param, zero,
                              entry);
        new CmpInstruction(CmpInstruction::LE, cmpDst, newParam, two, entry);
        BasicBlock* trueBranch = new BasicBlock(func);
        BasicBlock* falseBranch = new BasicBlock(func);
        entry->addSucc(trueBranch);
        entry->addSucc(falseBranch);
        trueBranch->addPred(entry);
        falseBranch->addPred(entry);
        new CondBrInstruction(trueBranch, falseBranch, cmpDst, entry);
        new RetInstruction(one, trueBranch);
        auto dst = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        new BinaryInstruction(BinaryInstruction::SUB, dst, newParam, one,
                              falseBranch);
        auto res1 = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        new CallInstruction(res1, func->getSymPtr(), {dst}, falseBranch);
        dst = new Operand(new TemporarySymbolEntry(TypeSystem::intType,
                                                   SymbolTable::getLabel()));
        new BinaryInstruction(BinaryInstruction::SUB, dst, newParam, two,
                              falseBranch);
        auto res2 = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        new CallInstruction(res2, func->getSymPtr(), {dst}, falseBranch);
        dst = new Operand(new TemporarySymbolEntry(TypeSystem::intType,
                                                   SymbolTable::getLabel()));
        new BinaryInstruction(BinaryInstruction::ADD, dst, res1, res2,
                              falseBranch);
        new RetInstruction(dst, falseBranch);
        return;
    }

    builder->setInsertBB(entry);
    if (decl)
        decl->genCode();
    // function中的stmt节点是用compoundstmt进行初始化的
    if (stmt)
        stmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors
     * for each basic block. Todo
     */

    // BasicBlock* void_retbb=new BasicBlock(func);
    // new RetInstruction(nullptr, void_retbb);
    for (auto block = func->begin(); block != func->end(); block++) {
        //获取该块的最后一条指令
        Instruction* i = (*block)->begin();
        Instruction* last = (*block)->rbegin();
        while (i != last) {
            if (i->isCond() || i->isUncond()) {
                (*block)->remove(i);
            }
            i = i->getNext();
        }
        if (last->isCond()) {
            BasicBlock *truebranch, *falsebranch;
            truebranch =
                dynamic_cast<CondBrInstruction*>(last)->getTrueBranch();
            falsebranch =
                dynamic_cast<CondBrInstruction*>(last)->getFalseBranch();
            if (truebranch->empty()) {
                new RetInstruction(nullptr, truebranch);

            } else if (falsebranch->empty()) {
                new RetInstruction(nullptr, falsebranch);
            }
            (*block)->addSucc(truebranch);
            (*block)->addSucc(falsebranch);
            truebranch->addPred(*block);
            falsebranch->addPred(*block);
        } else if (last->isUncond()) {  //无条件跳转指令可获取跳转的目标块
            BasicBlock* dst =
                dynamic_cast<UncondBrInstruction*>(last)->getBranch();
            (*block)->addSucc(dst);
            dst->addPred(*block);
            if (dst->empty()) {
                if (((FunctionType*)(se->getType()))->getRetType() ==
                    TypeSystem::intType) {
                    new RetInstruction(new Operand(new ConstantSymbolEntry(
                                           TypeSystem::intType, 0)),
                                       dst);
                } else if (((FunctionType*)(se->getType()))->getRetType() ==
                           TypeSystem::floatType) {
                    new RetInstruction(new Operand(new ConstantSymbolEntry(
                                           TypeSystem::floatType, 0)),
                                       dst);
                } else if (((FunctionType*)(se->getType()))->getRetType() ==
                           TypeSystem::voidType) {
                    new RetInstruction(nullptr, dst);
                }
            }

        }
        //最后一条语句不是返回以及跳转
        else if (!last->isRet()) {
            if (((FunctionType*)(se->getType()))->getRetType() ==
                TypeSystem::voidType) {
                new RetInstruction(nullptr, *block);
            }
        }
    }
    // 如果已经有ret了，删除后面的指令
    for (auto it = func->begin(); it != func->end(); it++) {
        auto block = *it;
        bool flag = false;
        for (auto i = block->begin(); i != block->end(); i = i->getNext()) {
            if (flag) {
                block->remove(i);
                delete i;
                continue;
            }
            if (i->isRet())
                flag = true;
        }
        if (flag) {
            while (block->succ_begin() != block->succ_end()) {
                auto b = *(block->succ_begin());
                block->removeSucc(b);
                b->removePred(block);
            }
        }
    }
    while (true) {
        bool flag = false;
        for (auto it = func->begin(); it != func->end(); it++) {
            auto block = *it;
            if (block == func->getEntry())
                continue;
            if (block->getNumOfPred() == 0) {
                delete block;
                flag = true;
                break;
            }
        }
        if (!flag)
            break;
    }
}

BinaryExpr::BinaryExpr(SymbolEntry* se,
                       int op,
                       ExprNode* expr1,
                       ExprNode* expr2)
    : ExprNode(se, BINARYEXPR), op(op), expr1(expr1), expr2(expr2) {
    dst = new Operand(se);
    std::string op_str;
    switch (op) {
        case ADD:
            op_str = "+";
            break;
        case SUB:
            op_str = "-";
            break;
        case MUL:
            op_str = "*";
            break;
        case DIV:
            op_str = "/";
            break;
        case MOD:
            op_str = "%";
            break;
        case AND:
            op_str = "&&";
            break;
        case OR:
            op_str = "||";
            break;
        case LESS:
            op_str = "<";
            break;
        case LESSEQUAL:
            op_str = "<=";
            break;
        case GREATER:
            op_str = ">";
            break;
        case GREATEREQUAL:
            op_str = ">=";
            break;
        case EQUAL:
            op_str = "==";
            break;
        case NOTEQUAL:
            op_str = "!=";
            break;
    }
    if (expr1->getType()->isVoid() || expr2->getType()->isVoid()) {
        fprintf(stderr,
                "invalid operand of type \'void\' to binary \'opeartor%s\'\n",
                op_str.c_str());
    }
    if (op >= BinaryExpr::AND && op <= BinaryExpr::NOTEQUAL) {
        type = TypeSystem::boolType;
        if (op == BinaryExpr::AND || op == BinaryExpr::OR) {
            if (expr1->getType()->isInt() &&
                expr1->getType()->getSize() == 32) {
                ImplicitCastExpr* temp = new ImplicitCastExpr(expr1);
                this->expr1 = temp;
            } else if (expr1->getType()->isFloat()) {
                SymbolEntry* zero =
                    new ConstantSymbolEntry(TypeSystem::floatType, 0);
                SymbolEntry* temp = new TemporarySymbolEntry(
                    TypeSystem::boolType, SymbolTable::getLabel());
                BinaryExpr* cmpZero = new BinaryExpr(temp, BinaryExpr::NOTEQUAL,
                                                     expr1, new Constant(zero));
                this->expr1 = cmpZero;
            }

            if (expr2->getType()->isInt() &&
                expr2->getType()->getSize() == 32) {
                ImplicitCastExpr* temp = new ImplicitCastExpr(expr2);
                this->expr2 = temp;
            } else if (expr2->getType()->isFloat()) {
                SymbolEntry* zero =
                    new ConstantSymbolEntry(TypeSystem::floatType, 0);
                SymbolEntry* temp = new TemporarySymbolEntry(
                    TypeSystem::boolType, SymbolTable::getLabel());
                BinaryExpr* cmpZero = new BinaryExpr(temp, BinaryExpr::NOTEQUAL,
                                                     expr2, new Constant(zero));
                this->expr2 = cmpZero;
            }
        }
        // TODO optimize literal number
        if (op == BinaryExpr::LESS || op == BinaryExpr::LESSEQUAL ||
            op == BinaryExpr::GREATER || op == BinaryExpr::GREATEREQUAL ||
            op == BinaryExpr::EQUAL || op == BinaryExpr::NOTEQUAL) {
            if (expr1->getType()->isFloat() && expr2->getType()->isInt()) {
                ImplicitCastExpr* temp =
                    new ImplicitCastExpr(expr2, TypeSystem::floatType);
                this->expr2 = temp;
            } else if (expr1->getType()->isInt() &&
                       expr2->getType()->isFloat()) {
                ImplicitCastExpr* temp =
                    new ImplicitCastExpr(expr1, TypeSystem::floatType);
                this->expr1 = temp;
                type = TypeSystem::floatType;
            }
        }
    } else if (expr1->getType()->isFloat() && expr2->getType()->isInt()) {
        if (op == BinaryExpr::MOD) {
            fprintf(stderr, "Operands of `mod` must be both integers");
        }
        ImplicitCastExpr* temp =
            new ImplicitCastExpr(expr2, TypeSystem::floatType);
        this->expr2 = temp;
        type = TypeSystem::floatType;
    } else if (expr1->getType()->isInt() && expr2->getType()->isFloat()) {
        if (op == BinaryExpr::MOD) {
            fprintf(stderr, "Operands of `mod` must be both integers");
        }
        ImplicitCastExpr* temp =
            new ImplicitCastExpr(expr1, TypeSystem::floatType);
        this->expr1 = temp;
        type = TypeSystem::floatType;
    } else if (expr1->getType()->isFloat() && expr2->getType()->isFloat()) {
        type = TypeSystem::floatType;
    } else {
        type = TypeSystem::intType;
    }
};

BinaryExpr::BinaryExpr(const BinaryExpr& b) : ExprNode(b) {
    op = b.op;
    expr1 = b.expr1->copy();
    expr2 = b.expr2->copy();
    symbolEntry = new TemporarySymbolEntry(b.symbolEntry->getType(),
                                           SymbolTable::getLabel());
    dst = new Operand(symbolEntry);
}

void BinaryExpr::genCode() {
    BasicBlock* bb = builder->getInsertBB();
    Function* func = bb->getParent();
    if (op == AND) {
        BasicBlock* trueBB = new BasicBlock(
            func);  // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(
            trueBB);  // set the insert point to the trueBB so that intructions
                      // generated by expr2 will be inserted into it.
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    } else if (op == OR) {
        // Todo
        BasicBlock* trueBB = new BasicBlock(func);
        expr1->genCode();
        backPatch(expr1->falseList(), trueBB);
        builder->setInsertBB(trueBB);
        expr2->genCode();
        true_list = merge(expr1->trueList(), expr2->trueList());
        false_list = expr2->falseList();
    } else if (op >= LESS && op <= NOTEQUAL) {
        // Todo
        expr1->genCode();
        expr2->genCode();
        Operand* src1 = expr1->getOperand();
        Operand* src2 = expr2->getOperand();
        if (src1->getType()->getSize() == 1) {
            Operand* dst = new Operand(new TemporarySymbolEntry(
                TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(dst, src1, bb);
            src1 = dst;
        }
        if (src2->getType()->getSize() == 1) {
            Operand* dst = new Operand(new TemporarySymbolEntry(
                TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(dst, src2, bb);
            src2 = dst;
        }
        int cmpopcode = -1;
        switch (op) {
            case LESS:
                cmpopcode = CmpInstruction::L;
                break;
            case LESSEQUAL:
                cmpopcode = CmpInstruction::LE;
                break;
            case GREATER:
                cmpopcode = CmpInstruction::G;
                break;
            case GREATEREQUAL:
                cmpopcode = CmpInstruction::GE;
                break;
            case EQUAL:
                cmpopcode = CmpInstruction::E;
                break;
            case NOTEQUAL:
                cmpopcode = CmpInstruction::NE;
                break;
        }
        new CmpInstruction(cmpopcode, dst, src1, src2, bb);
        //
        BasicBlock *truebb, *falsebb, *tempbb;
        //临时假块
        truebb = new BasicBlock(func);
        falsebb = new BasicBlock(func);
        tempbb = new BasicBlock(func);

        true_list.push_back(new CondBrInstruction(truebb, tempbb, dst, bb));

        false_list.push_back(new UncondBrInstruction(falsebb, tempbb));
    } else if (op >= ADD && op <= MOD) {
        expr1->genCode();
        expr2->genCode();
        Operand* src1 = expr1->getOperand();
        Operand* src2 = expr2->getOperand();
        int opcode = -1;
        switch (op) {
            case ADD:
                opcode = BinaryInstruction::ADD;
                break;
            case SUB:
                opcode = BinaryInstruction::SUB;
                break;
            case MUL:
                opcode = BinaryInstruction::MUL;
                break;
            case DIV:
                opcode = BinaryInstruction::DIV;
                break;
            case MOD:
                opcode = BinaryInstruction::MOD;
                break;
        }
        // if(opcode == BinaryInstruction::MOD && !(expr2->getSymbolEntry()->isConstant() && (int(expr2->getValue()) & int(expr2->getValue() - 1)) == 0)){
        //     // c1 = a / b
        //     Operand* dst0 = new Operand(new TemporarySymbolEntry(
        //         TypeSystem::intType, SymbolTable::getLabel()));
        //     Operand* dst1 = new Operand(new TemporarySymbolEntry(
        //         TypeSystem::intType, SymbolTable::getLabel()));
        //     new BinaryInstruction(BinaryInstruction::DIV, dst0, src1, src2, bb);
        //     // c2 = c1 * b
        //     new BinaryInstruction(BinaryInstruction::MUL, dst1, dst0, src2, bb);
        //     // c = a - c2
        //     new BinaryInstruction(BinaryInstruction::SUB, dst, src1, dst1, bb);
        // }
        // else
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

ExprNode* BinaryExpr::getLeft() {
    return this->expr1;
}

ExprNode* BinaryExpr::getRight() {
    return this->expr2;
}

ExprNode* UnaryExpr::getSubExpr() {
    return this->expr;
}
void Constant::genCode() {
    // we don't need to generate code.
}

void Id::genCode() {
    BasicBlock* bb = builder->getInsertBB();
    Operand* addr =
        dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    if (type->isInt() || type->isFloat()) {
        new LoadInstruction(dst, addr, bb);
    } else if (type->isArray()) {
        if (arrIdx) {
            Type* type = ((ArrayType*)(this->type))->getElementType();
            Type* type1 = this->type;
            Operand* tempSrc = addr;
            Operand* tempDst = dst;
            ExprNode* idx = arrIdx;
            bool flag = false;
            bool pointer = false;
            bool firstFlag = true;
            while (true) {
                if (((ArrayType*)type1)->getLength() == -1) {
                    Operand* dst1 = new Operand(new TemporarySymbolEntry(
                        new PointerType(type), SymbolTable::getLabel()));
                    tempSrc = dst1;
                    new LoadInstruction(dst1, addr, bb);
                    flag = true;
                    firstFlag = false;
                }
                if (!idx) {
                    Operand* dst1 = new Operand(new TemporarySymbolEntry(
                        new PointerType(type), SymbolTable::getLabel()));
                    Operand* idx = new Operand(
                        new ConstantSymbolEntry(TypeSystem::intType, 0));
                    new GepInstruction(dst1, tempSrc, idx, bb);
                    tempDst = dst1;
                    pointer = true;
                    break;
                }

                idx->genCode();
                auto gep = new GepInstruction(tempDst, tempSrc,
                                              idx->getOperand(), bb, flag);
                if (!flag && firstFlag) {
                    gep->setFirst();
                    firstFlag = false;
                }
                if (flag)
                    flag = false;
                if (type == TypeSystem::intType ||
                    type == TypeSystem::constIntType ||
                    type == TypeSystem::floatType ||
                    type == TypeSystem::constFloatType) {
                    break;
                }
                type = ((ArrayType*)type)->getElementType();
                type1 = ((ArrayType*)type1)->getElementType();
                tempSrc = tempDst;
                tempDst = new Operand(new TemporarySymbolEntry(
                    new PointerType(type), SymbolTable::getLabel()));
                idx = (ExprNode*)(idx->getNext());
            }
            dst = tempDst;
            // 如果是右值还需要一条load
            if (!left && !pointer) {
                Operand* dst1 = new Operand(
                    new TemporarySymbolEntry(type, SymbolTable::getLabel()));
                new LoadInstruction(dst1, dst, bb);
                dst = dst1;
            }

        } else {
            // 这里先这么办 后续有问题再改
            if (((ArrayType*)(this->type))->getLength() == -1) {
                Operand* dst1 = new Operand(new TemporarySymbolEntry(
                    new PointerType(
                        ((ArrayType*)(this->type))->getElementType()),
                    SymbolTable::getLabel()));
                new LoadInstruction(dst1, addr, bb);
                dst = dst1;

            } else {
                Operand* idx = new Operand(
                    new ConstantSymbolEntry(TypeSystem::intType, 0));
                auto gep = new GepInstruction(dst, addr, idx, bb);
                gep->setFirst();
            }
        }
    }
}

void IfStmt::genCode() {
    Function* func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();

    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode() {
    Function* func;
    BasicBlock *then_bb, *else_bb, *end_bb /*, *bb*/;
    // bb = builder->getInsertBB();
    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    else_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();
    // Operand* IfElsecond = cond->getOperand();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), else_bb);

    // new CondBrInstruction(then_bb,else_bb,IfElsecond,bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);

    builder->setInsertBB(end_bb);
}

void CompoundStmt::genCode() {
    // Todo
    if (stmt)
        stmt->genCode();
}

void SeqNode::genCode() {
    // Todo
    stmt1->genCode();
    stmt2->genCode();
}

bool DeclStmt::hasMemset = false;

void DeclStmt::genCode() {
    IdentifierSymbolEntry* se =
        dynamic_cast<IdentifierSymbolEntry*>(id->getSymbolEntry());
    if (se->isGlobal()) {
        Operand* addr;
        SymbolEntry* addr_se;
        addr_se = new IdentifierSymbolEntry(*se);
        addr_se->setType(new PointerType(se->getType()));
        addr = new Operand(addr_se);
        se->setAddr(addr);
        unit.insertGlobal(se);
        mUnit.insertGlobal(se);
    } else if (se->isLocal() || se->isParam()) {
        Function* func = builder->getInsertBB()->getParent();
        BasicBlock* entry = func->getEntry();
        Instruction* alloca;
        Operand* addr;
        SymbolEntry* addr_se;
        Type* type;
        // if (se->isParam() && se->getType()->isArray())
        //     type = new PointerType(TypeSystem::intType);
        // else
        type = new PointerType(se->getType());                              //yr
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());  //yr
        // addr_se = se; //yr
        addr = new Operand(addr_se);
        alloca = new AllocaInstruction(addr, se);
        // allocate space for local id in function stack.
        entry->addAlloc(alloca);  // allocate instructions should be inserted
                                  // into the begin of the entry block.
        Operand* temp = nullptr;
        if (se->isParam())
            temp = se->getAddr();
        se->setAddr(addr);  // set the addr operand in symbol entry so that
                            // we can use it in subsequent code generation.
                            // can use it in subsequent code generation.
        bool useMemset = false;
        if (expr && se->getType()->isArray()) {
            int notZeroNum = se->getNotZeroNum();
            int size = se->getType()->getSize() / 8;
            int length = size / 4;
            if (notZeroNum < length / 2) {
                useMemset = true;
                auto int8PtrType = new PointerType(TypeSystem::int8Type);
                Operand* int8Ptr = new Operand(new TemporarySymbolEntry(
                    int8PtrType, SymbolTable::getLabel()));
                auto bb = builder->getInsertBB();
                new BitcastInstruction(int8Ptr, addr, bb);
                std::string name = "llvm.memset.p0.i32";
                SymbolEntry* funcSE;
                if (!hasMemset) {
                    hasMemset = true;
                    std::vector<Type*> vec;
                    vec.push_back(int8PtrType);
                    vec.push_back(TypeSystem::int8Type);
                    vec.push_back(TypeSystem::intType);
                    vec.push_back(TypeSystem::boolType);
                    std::vector<SymbolEntry*> vec1;
                    auto funcType =
                        new FunctionType(TypeSystem::voidType, vec, vec1);
                    SymbolTable* st = identifiers;
                    while (st->getPrev())
                        st = st->getPrev();
                    funcSE = new IdentifierSymbolEntry(funcType, name,
                                                       st->getLevel());
                    st->install(name, funcSE);
                    unit.insertDeclare(funcSE);
                } else {
                    funcSE = identifiers->lookup(name);
                    assert(funcSE);
                }
                std::vector<Operand*> params;
                params.push_back(int8Ptr);
                params.push_back(new Operand(
                    new ConstantSymbolEntry(TypeSystem::int8Type, 0)));
                params.push_back(new Operand(
                    new ConstantSymbolEntry(TypeSystem::intType, size)));
                params.push_back(new Operand(
                    new ConstantSymbolEntry(TypeSystem::boolType, 0)));
                new CallInstruction(nullptr, funcSE, params, bb);
            }
        }
        if (expr) {
            if (expr->isInitValueListExpr()) {
                Operand* init = nullptr;
                int off = 0;
                BasicBlock* bb = builder->getInsertBB();
                ExprNode* temp = expr;
                std::stack<ExprNode*> stk;
                std::vector<int> idx;
                idx.push_back(0);
                while (temp) {
                    if (temp->isInitValueListExpr()) {
                        stk.push(temp);
                        idx.push_back(0);
                        temp = ((InitValueListExpr*)temp)->getExpr();
                        continue;
                    } else {
                        temp->genCode();
                        Type* type =
                            ((ArrayType*)(se->getType()))->getElementType();
                        Operand* tempSrc = addr;
                        Operand* tempDst;
                        Operand* index;
                        bool flag = true;
                        int i = 1;
                        while (true) {
                            tempDst = new Operand(new TemporarySymbolEntry(
                                new PointerType(type),
                                SymbolTable::getLabel()));
                            index = (new Constant(new ConstantSymbolEntry(
                                         TypeSystem::intType, idx[i++])))
                                        ->getOperand();
                            auto gep =
                                new GepInstruction(tempDst, tempSrc, index, bb);
                            // if (!useMemset)
                            gep->setInit(init, off);
                            if (flag) {
                                gep->setFirst();
                                flag = false;
                            }
                            if (type == TypeSystem::intType ||
                                type == TypeSystem::constIntType ||
                                type == TypeSystem::floatType ||
                                type == TypeSystem::constFloatType) {
                                gep->setLast();
                                break;
                            }
                            type = ((ArrayType*)type)->getElementType();
                            tempSrc = tempDst;
                        }
                        if (useMemset &&
                            temp->getOperand()->getEntry()->isConstant() &&
                            ((ConstantSymbolEntry*)(temp->getOperand()
                                                        ->getEntry()))
                                    ->getValue() == 0) {
                            bb->deleteBack(idx.size() - 1);
                            off += 4;
                        } else {
                            init = tempDst;
                            off = 0;
                            new StoreInstruction(tempDst, temp->getOperand(),
                                                 bb);
                        }
                    }
                    while (true) {
                        if (temp->getNext()) {
                            temp = (ExprNode*)(temp->getNext());
                            idx[idx.size() - 1]++;
                            break;
                        } else {
                            temp = stk.top();
                            stk.pop();
                            idx.pop_back();
                            if (stk.empty())
                                break;
                        }
                    }
                    if (stk.empty())
                        break;
                }
            } else {
                BasicBlock* bb = builder->getInsertBB();
                expr->genCode();
                Operand* src = expr->getOperand();
                new StoreInstruction(addr, src, bb);
            }
        }
        if (se->isParam()) {
            BasicBlock* bb = builder->getInsertBB();
            new StoreInstruction(addr, temp, bb);
        }
    }
    if (this->getNext()) {
        this->getNext()->genCode();
    }
}

void ReturnStmt::genCode() {
    // Todo
    BasicBlock* bb = builder->getInsertBB();
    Operand* src = nullptr;
    if (retValue) {
        retValue->genCode();
        src = retValue->getOperand();
    }
    new RetInstruction(src, bb);
}
void ExprStmt::genCode() {
    // Todo
    expr->genCode();
}
void ContinueStmt::genCode() {
    // Todo
    Function* func = builder->getInsertBB()->getParent();
    BasicBlock* bb = builder->getInsertBB();
    new UncondBrInstruction(((WhileStmt*)whileStmt)->get_cond_bb(), bb);
    BasicBlock* continue_next_bb = new BasicBlock(func);
    builder->setInsertBB(continue_next_bb);
}
void BreakStmt::genCode() {
    // Todo
    Function* func = builder->getInsertBB()->getParent();
    BasicBlock* bb = builder->getInsertBB();
    new UncondBrInstruction(((WhileStmt*)whileStmt)->get_end_bb(), bb);
    BasicBlock* break_next_bb = new BasicBlock(func);
    builder->setInsertBB(break_next_bb);
}
void WhileStmt::genCode() {
    // Function* func;
    // BasicBlock *cond_bb, *while_bb, *end_bb, *bb;
    // bb = builder->getInsertBB();
    // func = builder->getInsertBB()->getParent();
    // cond_bb = new BasicBlock(func);
    // while_bb = new BasicBlock(func);
    // end_bb = new BasicBlock(func);

    // this->cond_bb = cond_bb;
    // this->end_bb = end_bb;

    // new UncondBrInstruction(cond_bb, bb);

    // builder->setInsertBB(cond_bb);
    // cond->genCode();
    // backPatch(cond->trueList(), while_bb);
    // backPatch(cond->falseList(), end_bb);
    // // Operand* condoperand= cond->getOperand();
    // // new CondBrInstruction(while_bb,end_bb,condoperand,cond_bb);

    // builder->setInsertBB(while_bb);
    // stmt->genCode();

    // while_bb = builder->getInsertBB();
    // new UncondBrInstruction(cond_bb, while_bb);

    // builder->setInsertBB(end_bb);

    Function* func;
    BasicBlock *cond_bb, *while_bb, *end_bb, *bb;
    bb = builder->getInsertBB();
    func = builder->getInsertBB()->getParent();
    cond_bb = new BasicBlock(func);
    while_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    this->cond_bb = cond_bb;
    this->end_bb = end_bb;

    new UncondBrInstruction(cond_bb, bb);

    builder->setInsertBB(cond_bb);
    cond->genCode();
    backPatch(cond->trueList(), while_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(while_bb);
    stmt->genCode();
    ExprNode* cond1 = cond->copy();
    // ExprNode* cond1 = cond;
    cond1->genCode();
    backPatch(cond1->trueList(), while_bb);
    backPatch(cond1->falseList(), end_bb);

    // Operand* condoperand = cond->getOperand();
    // auto end = ((CondBrInstruction*)(cond_bb->rbegin()))->getFalseBranch();
    // new CondBrInstruction(while_bb, end, condoperand,
    //                       builder->getInsertBB());
    // std::vector<Instruction*>().swap(cond->trueList());
    // cond-

    // while_bb = builder->getInsertBB();
    // new UncondBrInstruction(cond_bb, while_bb);

    builder->setInsertBB(end_bb);
}
void BlankStmt::genCode() {
    // Todo
}
void InitValueListExpr::genCode() {
    // Todo
}
void CallExpr::genCode() {
    std::vector<Operand*> operands;
    ExprNode* temp = param;
    while (temp) {
        temp->genCode();
        operands.push_back(temp->getOperand());
        temp = ((ExprNode*)temp->getNext());
    }
    BasicBlock* bb = builder->getInsertBB();
    new CallInstruction(dst, symbolEntry, operands, bb);
}
void UnaryExpr::genCode() {
    // Todo
    expr->genCode();
    if (op == NOT) {
        BasicBlock* bb = builder->getInsertBB();
        Operand* src = expr->getOperand();
        if (expr->getType()->isFloat() ||
            expr->getType()->isInt()) {  // FIXME: not i1
            Operand* temp = new Operand(new TemporarySymbolEntry(
                TypeSystem::boolType, SymbolTable::getLabel()));
            new CmpInstruction(
                CmpInstruction::NE, temp, src,
                new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)),
                bb);
            src = temp;
        }
        new XorInstruction(dst, src, bb);
    } else if (op == SUB) {
        Operand* src2;
        BasicBlock* bb = builder->getInsertBB();
        Operand* src1 = new Operand(new ConstantSymbolEntry(dst->getType(), 0));
        if (expr->getType()->getSize() == 1) {
            src2 = new Operand(new TemporarySymbolEntry(
                TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(src2, expr->getOperand(), bb);
        } else
            src2 = expr->getOperand();
        new BinaryInstruction(BinaryInstruction::SUB, dst, src1, src2, bb);
    }
}
void ExprNode::genCode() {
    // Todo
}

ExprNode* ExprNode::alge_simple(int depth) {
    /* simplification rules:
    a*1=a, a*0=0, a/1=a, a+0=a, a-0=a, b||false=b, b&&true=b
    */
    int op;
    ExprNode* res = this;
    if (this->isBinaryExpr()) {
        enum {
            ADD,
            SUB,
            MUL,
            DIV,
            MOD,
            AND,
            OR,
            LESS,
            LESSEQUAL,
            GREATER,
            GREATEREQUAL,
            EQUAL,
            NOTEQUAL
        };
        op = ((BinaryExpr*)this)->getOp();
        ExprNode *lhs = ((BinaryExpr*)this)->getLeft(),
                 *rhs = ((BinaryExpr*)this)->getRight();
        if (depth && lhs->isBinaryExpr()) {
            lhs = lhs->alge_simple(depth - 1);
        }
        if (depth && rhs->isBinaryExpr()) {
            rhs = rhs->alge_simple(depth - 1);
        }
        switch (op) {
            case ADD:
                if (lhs->getSymbolEntry()->isConstant() &&
                    ((ConstantSymbolEntry*)(lhs->getSymbolEntry()))
                            ->getValue() == 0) {
                    res = rhs;
                } else if (rhs->getSymbolEntry()->isConstant() &&
                           ((ConstantSymbolEntry*)(rhs->getSymbolEntry()))
                                   ->getValue() == 0) {
                    res = lhs;
                } else {
                    SymbolEntry* se =
                        new TemporarySymbolEntry(type, SymbolTable::getLabel());
                    res = new BinaryExpr(se, ADD, lhs, rhs);
                }
                break;
            case SUB:
                if (rhs->getSymbolEntry()->isConstant() &&
                    ((ConstantSymbolEntry*)(rhs->getSymbolEntry()))
                            ->getValue() == 0) {
                    res = lhs;
                } else {
                    SymbolEntry* se =
                        new TemporarySymbolEntry(type, SymbolTable::getLabel());
                    res = new BinaryExpr(se, SUB, lhs, rhs);
                }
                break;
            case MUL:
                if (lhs->getSymbolEntry()->isConstant()) {
                    if (((ConstantSymbolEntry*)(lhs->getSymbolEntry()))
                            ->getValue() == 0) {
                        SymbolEntry* se = new ConstantSymbolEntry(type, 0);
                        res = new Constant(se);
                    } else if (((ConstantSymbolEntry*)(lhs->getSymbolEntry()))
                                   ->getValue() == 1) {
                        res = rhs;
                    }
                } else if (rhs->getSymbolEntry()->isConstant()) {
                    if (((ConstantSymbolEntry*)(rhs->getSymbolEntry()))
                            ->getValue() == 0) {
                        SymbolEntry* se = new ConstantSymbolEntry(type, 0);
                        res = new Constant(se);
                    } else if (((ConstantSymbolEntry*)(rhs->getSymbolEntry()))
                                   ->getValue() == 1) {
                        res = lhs;
                    }
                } else {
                    SymbolEntry* se =
                        new TemporarySymbolEntry(type, SymbolTable::getLabel());
                    res = new BinaryExpr(se, MUL, lhs, rhs);
                }
                break;
            case DIV:
                if (rhs->getSymbolEntry()->isConstant() &&
                    ((ConstantSymbolEntry*)(lhs->getSymbolEntry()))
                            ->getValue() == 1) {
                    res = lhs;
                } else {
                    SymbolEntry* se =
                        new TemporarySymbolEntry(type, SymbolTable::getLabel());
                    res = new BinaryExpr(se, DIV, lhs, rhs);
                }
                break;
                /*
        case AND:
            if(lhs->getSymbolEntry()->isConstant()){
                if(((ConstantSymbolEntry*)(lhs->getSymbolEntry()))->getValue()==true){
                    res = rhs;
                }else if(((ConstantSymbolEntry*)(lhs->getSymbolEntry()))->getValue()==false){
                    SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::boolType, 0);
                    res = new Constant(se);
                }
            }   
            else if(rhs->getSymbolEntry()->isConstant()){
                if(((ConstantSymbolEntry*)(rhs->getSymbolEntry()))->getValue()==true){
                    res = lhs;
                }else if(((ConstantSymbolEntry*)(rhs->getSymbolEntry()))->getValue()==false){
                    SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::boolType, 0);
                    res = new Constant(se);
                }
            }   
            else{
                SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
                res = new BinaryExpr(se, AND, lhs, rhs);
            }
            break;
        case OR:
            if(lhs->getSymbolEntry()->isConstant()){
                if(((ConstantSymbolEntry*)(lhs->getSymbolEntry()))->getValue()==true){
                    SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::boolType, 1);
                    res = new Constant(se);
                }else if(((ConstantSymbolEntry*)(lhs->getSymbolEntry()))->getValue()==false){
                    res = rhs;
                }
            }   
            else if(rhs->getSymbolEntry()->isConstant()){
                if(((ConstantSymbolEntry*)(rhs->getSymbolEntry()))->getValue()==true){
                    SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::boolType, 1);
                    res = new Constant(se);
                }else if(((ConstantSymbolEntry*)(rhs->getSymbolEntry()))->getValue()==false){
                    res = lhs;
                }
            }   
            else{
                SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
                res = new BinaryExpr(se, OR, lhs, rhs);
            }
            break;*/
        }
    }
    return res;
}

ExprNode* ExprNode::const_fold() {
    ExprNode* res = this;
    res = this->alge_simple(5);  // 代数化简
    bool flag = true;
    double fconst = res->fold_const(flag);
    if (flag) {
        if(dst->getType()->isInt())
            fconst = int(fconst);
        SymbolEntry* se = new ConstantSymbolEntry(type, fconst);
        res = new Constant(se);
    }
    return res;
}

double ExprNode::fold_const(bool& flag) {
    if (this->isBinaryExpr()) {
        ExprNode *lhs = ((BinaryExpr*)this)->getLeft(),
                 *rhs = ((BinaryExpr*)this)->getRight();
        lhs->fold_const(flag);
        if (flag)
            rhs->fold_const(flag);
        if (flag) {
            return ((BinaryExpr*)this)->getValue();
        } else
            return 0;
    } else if (this->isUnaryExpr()) {
        ExprNode* hs = ((UnaryExpr*)this)->getSubExpr();
        hs->fold_const(flag);
        if (flag) {
            return ((UnaryExpr*)this)->getValue();
        } else
            return 0;
    } else if (this->isExpr() && this->getSymbolEntry()->isConstant()) {
        return ((ConstantSymbolEntry*)(this->getSymbolEntry()))->getValue();
    }
    flag = 0;
    return 0;
}

bool ContinueStmt::typeCheck(Type* retType) {
    return false;
}
bool BreakStmt::typeCheck(Type* retType) {
    return false;
}
bool WhileStmt::typeCheck(Type* retType) {
    if (stmt)
        return stmt->typeCheck(retType);
    return false;
}
bool BlankStmt::typeCheck(Type* retType) {
    return false;
}
bool InitValueListExpr::typeCheck(Type* retType) {
    return false;
}
bool CallExpr::typeCheck(Type* retType) {
    return false;
}
bool UnaryExpr::typeCheck(Type* retType) {
    return false;
}

bool ExprStmt::typeCheck(Type* retType) {
    return false;
}

void AssignStmt::genCode() {
    BasicBlock* bb = builder->getInsertBB();
    expr->genCode();
    Operand* addr = nullptr;
    if (lval->getOriginType()->isInt() || lval->getOriginType()->isFloat())
        addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymbolEntry())
                   ->getAddr();
    else if (lval->getOriginType()->isArray()) {
        ((Id*)lval)->setLeft();
        lval->genCode();
        addr = lval->getOperand();
        // Type* type = new PointerType(TypeSystem::intType);
        // SymbolEntry* addr_se = new TemporarySymbolEntry(type,
        // SymbolTable::getLabel()); addr = new Operand(addr_se);
    }
    Operand* src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just
     * store the result of the `expr` to the addr of the id. If you want to
     * implement array, you have to caculate the address first and then store
     * the result into it.
     */
    new StoreInstruction(addr, src, bb);
}

bool Ast::typeCheck(Type* retType) {
    if (root != nullptr)
        return root->typeCheck();
    return false;
}

bool FunctionDef::typeCheck(Type* retType) {
    SymbolEntry* se = this->getSymbolEntry();
    Type* ret = ((FunctionType*)(se->getType()))->getRetType();
    StmtNode* stmt = this->stmt;
    if (stmt == nullptr) {
        if (ret != TypeSystem::voidType)
            fprintf(stderr, "non-void function does not return a value.\n");
        // 不嵌套函数定义就返回了
        return false;
    }
    if (!stmt->typeCheck(ret)) {
        // fprintf(stderr, "function does not have a return statement\n");
        // TODO
        return false;
    }
    return false;
}

bool BinaryExpr::typeCheck(Type* retType) {
    return false;
}

bool Constant::typeCheck(Type* retType) {
    return false;
}

bool Id::typeCheck(Type* retType) {
    // 没有定义的话还是由生成树节点的时候完成吧，否则变量名拿不到
    // 重复定义也这样了
    return false;
}

bool IfStmt::typeCheck(Type* retType) {
    if (thenStmt)
        return thenStmt->typeCheck(retType);
    return false;
}

bool IfElseStmt::typeCheck(Type* retType) {
    bool flag1 = false, flag2 = false;
    if (thenStmt)
        flag1 = thenStmt->typeCheck(retType);
    if (elseStmt)
        flag2 = elseStmt->typeCheck(retType);
    return flag1 || flag2;
}

bool CompoundStmt::typeCheck(Type* retType) {
    if (stmt)
        return stmt->typeCheck(retType);
    return false;
}

bool SeqNode::typeCheck(Type* retType) {
    bool flag1 = false, flag2 = false;
    if (stmt1)
        flag1 = stmt1->typeCheck(retType);
    if (stmt2)
        flag2 = stmt2->typeCheck(retType);
    return flag1 || flag2;
}

bool DeclStmt::typeCheck(Type* retType) {
    return false;
}

bool ReturnStmt::typeCheck(Type* retType) {
    if (!retType) {
        fprintf(stderr, "expected unqualified-id\n");
        return true;
    }
    if (!retValue && !retType->isVoid()) {
        fprintf(
            stderr,
            "return-statement with no value, in function returning \'%s\'\n",
            retType->toStr().c_str());
        return true;
    }
    if (retValue && retType->isVoid()) {
        fprintf(
            stderr,
            "return-statement with a value, in function returning \'void\'\n");
        return true;
    }

    if (!retType->isVoid()) {
        Type* type = retValue->getType();
        if (type != retType) {
            fprintf(
                stderr,
                "cannot initialize return object of type \'%s\' with an rvalue "
                "of type \'%s\'\n",
                retType->toStr().c_str(), type->toStr().c_str());
            return true;
        }
    }
    return false;
}

bool AssignStmt::typeCheck(Type* retType) {
    return false;
}

CallExpr::CallExpr(SymbolEntry* se, ExprNode* param)
    : ExprNode(se, CALLEXPR), param(param) {
    // 做参数的检查
    dst = nullptr;
    SymbolEntry* s = se;
    int paramCnt = 0;
    ExprNode* temp = param;
    while (temp) {
        paramCnt++;
        temp = (ExprNode*)(temp->getNext());
    }
    while (s) {
        Type* type = s->getType();
        std::vector<Type*> params = ((FunctionType*)type)->getParamsType();
        if ((long unsigned int)paramCnt == params.size()) {
            this->symbolEntry = s;
            break;
        }
        s = s->getNext();
    }
    if (symbolEntry) {
        Type* type = symbolEntry->getType();
        this->type = ((FunctionType*)type)->getRetType();
        if (this->type != TypeSystem::voidType) {
            SymbolEntry* se =
                new TemporarySymbolEntry(this->type, SymbolTable::getLabel());
            dst = new Operand(se);
        }
        std::vector<Type*> params = ((FunctionType*)type)->getParamsType();
        ExprNode* temp = param;
        ExprNode* temp1 = nullptr;

        for (auto it = params.begin(); it != params.end(); it++) {
            if (temp == nullptr) {
                fprintf(stderr, "too few arguments to function %s %s\n",
                        symbolEntry->toStr().c_str(), type->toStr().c_str());
                break;
            } else if ((*it)->getKind() != temp->getType()->getKind()) {
                if (((*it)->isFloat() && temp->getType()->isInt()) ||
                    ((*it)->isInt() && temp->getType()->isFloat())) {
                    ImplicitCastExpr* implicitCastExpr =
                        new ImplicitCastExpr(temp, (*it));
                    implicitCastExpr->setNext((ExprNode*)(temp->getNext()));

                    if (it != params.begin()) {
                        temp1->setAdjNext(implicitCastExpr);
                        temp = implicitCastExpr;
                    } else {
                        temp = implicitCastExpr;
                        this->param = implicitCastExpr;
                    }

                } else {
                    fprintf(stderr, "parameter's type %s can't convert to %s\n",
                            temp->getType()->toStr().c_str(),
                            (*it)->toStr().c_str());
                }
            }
            temp1 = temp;
            temp = (ExprNode*)(temp->getNext());
        }
        if (temp != nullptr) {
            fprintf(stderr, "too many arguments to function %s %s\n",
                    symbolEntry->toStr().c_str(), type->toStr().c_str());
        }
    }
    if (((IdentifierSymbolEntry*)se)->isSysy()) {
        unit.insertDeclare(se);
    }
}

CallExpr::CallExpr(const CallExpr& c) : ExprNode(c) {
    if (c.param)
        param = c.param->copy();
    symbolEntry = c.symbolEntry;
    if (symbolEntry) {
        Type* type = symbolEntry->getType();
        this->type = ((FunctionType*)type)->getRetType();
        if (this->type != TypeSystem::voidType) {
            SymbolEntry* se =
                new TemporarySymbolEntry(this->type, SymbolTable::getLabel());
            dst = new Operand(se);
        }
    }
}

AssignStmt::AssignStmt(ExprNode* lval, ExprNode* expr)
    : lval(lval), expr(expr) {
    Type* type = ((Id*)lval)->getType();
    Type* exprType = expr->getType();
    SymbolEntry* se = lval->getSymbolEntry();
    bool flag = true;
    if (type->isInt()) {
        if (((IntType*)type)->isConst()) {
            fprintf(stderr,
                    "cannot assign to variable \'%s\' with const-qualified "
                    "type \'%s\'\n",
                    ((IdentifierSymbolEntry*)se)->toStr().c_str(),
                    type->toStr().c_str());
            flag = false;
        }
    } else if (type->isFloat()) {
        if (((FloatType*)type)->isConst()) {
            fprintf(stderr,
                    "cannot assign to variable \'%s\' with const-qualified "
                    "type \'%s\'\n",
                    ((IdentifierSymbolEntry*)se)->toStr().c_str(),
                    type->toStr().c_str());
            flag = false;
        }
    } else if (type->isArray()) {
        fprintf(stderr, "array type \'%s\' is not assignable\n",
                type->toStr().c_str());
        flag = false;
    }
    if (flag) {
        if (type != exprType) {  // comparing ptr, how about const?
            if (type->isInt() && exprType->isFloat()) {
                ImplicitCastExpr* temp =
                    new ImplicitCastExpr(expr, TypeSystem::intType);
                this->expr = temp;
            } else if (type->isFloat() && exprType->isInt()) {
                ImplicitCastExpr* temp =
                    new ImplicitCastExpr(expr, TypeSystem::floatType);
                this->expr = temp;
            } else {
                fprintf(stderr,
                        "cannot initialize a variable of type \'%s\' with an "
                        "rvalue "
                        "of type \'%s\'\n",
                        type->toStr().c_str(), exprType->toStr().c_str());
            }
        }
    }
}

Type* Id::getType() {
    SymbolEntry* se = this->getSymbolEntry();
    if (!se)
        return TypeSystem::voidType;
    Type* type = se->getType();
    if (!arrIdx)
        return type;
    else if (!type->isArray()) {
        fprintf(stderr, "subscripted value is not an array\n");
        return TypeSystem::voidType;
    } else {
        ArrayType* temp1 = (ArrayType*)type;  // whole type of array
        ExprNode* temp2 = arrIdx;             // current index
        while (!temp1->getElementType()->isInt() &&
               !temp1->getElementType()->isFloat()) {
            if (!temp2) {
                return temp1;  // return array
            }
            temp2 = (ExprNode*)(temp2->getNext());
            temp1 = (ArrayType*)(temp1->getElementType());
        }
        if (!temp2) {
            fprintf(stderr, "subscripted value is not an array\n");
            return temp1;
        } else if (temp2->getNext()) {
            fprintf(stderr, "subscripted value is not an array\n");
            return TypeSystem::voidType;
        }
        // printf("[temp1]\t%s\n", temp1->getElementType()->toStr().c_str());

        return temp1->getElementType();  // should be ok, probably.
    }
}

void ExprNode::output(int level) {
    std::string name, type;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cconst string\ttype:%s\t%s\n", level, ' ', type.c_str(),
            name.c_str());
}

void Ast::output() {
    fprintf(yyout, "program\n");
    if (root != nullptr)
        root->output(4);
}

void BinaryExpr::output(int level) {
    std::string op_str;
    switch (op) {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case MUL:
            op_str = "mul";
            break;
        case DIV:
            op_str = "div";
            break;
        case MOD:
            op_str = "mod";
            break;
        case AND:
            op_str = "and";
            break;
        case OR:
            op_str = "or";
            break;
        case LESS:
            op_str = "less";
            break;
        case LESSEQUAL:
            op_str = "lessequal";
            break;
        case GREATER:
            op_str = "greater";
            break;
        case GREATEREQUAL:
            op_str = "greaterequal";
            break;
        case EQUAL:
            op_str = "equal";
            break;
        case NOTEQUAL:
            op_str = "notequal";
            break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\ttype: %s\n", level, ' ',
            op_str.c_str(), type->toStr().c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

double BinaryExpr::getValue() {
    double value = 0;
    if (type->isFloat()) {
        float val;
        float val1 = (float)(expr1->getValue());
        float val2 = (float)(expr2->getValue());

        switch (op) {
            case ADD:
                val = val1 + val2;
                break;
            case SUB:
                val = val1 - val2;
                break;
            case MUL:
                val = val1 * val2;
                break;
            case DIV:
                if (val2 != 0){
                    val = val1 / val2;
                    if(dst->getType()->isInt()){
                        val = (int)(val);
                    }                        
                }                    
                break;
            case MOD:
                val = (int)(val1) % (int)(val2);
                break;
            case AND:  // FIXME
                val = val1 && val2;
                break;
            case OR:  // FIXME
                val = val1 || val2;
                break;
            case LESS:
                val = val1 < val2;
                break;
            case LESSEQUAL:
                val = val1 <= val2;
                break;
            case GREATER:
                val = val1 > val2;
                break;
            case GREATEREQUAL:
                val = val1 >= val2;
                break;
            case EQUAL:
                val = val1 == val2;
                break;
            case NOTEQUAL:
                val = val1 != val2;
                break;
        }
        value = (double)val;
        return value;
    }

    switch (op) {
        case ADD:
            value = expr1->getValue() + expr2->getValue();
            break;
        case SUB:
            value = expr1->getValue() - expr2->getValue();
            break;
        case MUL:
            value = expr1->getValue() * expr2->getValue();
            break;
        case DIV:
            if (expr2->getValue())
                value = expr1->getValue() / expr2->getValue();
            break;
        case MOD:
            value = (int)(expr1->getValue()) % (int)(expr2->getValue());
            break;
        case AND:
            value = expr1->getValue() && expr2->getValue();
            break;
        case OR:
            value = expr1->getValue() || expr2->getValue();
            break;
        case LESS:
            value = expr1->getValue() < expr2->getValue();
            break;
        case LESSEQUAL:
            value = expr1->getValue() <= expr2->getValue();
            break;
        case GREATER:
            value = expr1->getValue() > expr2->getValue();
            break;
        case GREATEREQUAL:
            value = expr1->getValue() >= expr2->getValue();
            break;
        case EQUAL:
            value = expr1->getValue() == expr2->getValue();
            break;
        case NOTEQUAL:
            value = expr1->getValue() != expr2->getValue();
            break;
    }  // double should be ok here, probably...
    return value;
}

UnaryExpr::UnaryExpr(SymbolEntry* se, int op, ExprNode* expr)
    : ExprNode(se, UNARYEXPR), op(op), expr(expr) {
    std::string op_str = op == UnaryExpr::NOT ? "!" : "-";
    if (expr->getType()->isVoid()) {
        fprintf(stderr,
                "invalid operand of type \'void\' to unary \'opeartor%s\'\n",
                op_str.c_str());
    }
    if (op == UnaryExpr::NOT) {
        type = TypeSystem::intType;
        dst = new Operand(se);
        if (expr->isUnaryExpr()) {
            UnaryExpr* ue = (UnaryExpr*)expr;
            if (ue->getOp() == UnaryExpr::NOT) {
                if (ue->getType() == TypeSystem::intType)
                    ue->setType(TypeSystem::boolType);  // TODO: type?
                // type = TypeSystem::intType;
                // type casting for float is done in `parser.y`
            }
        }
        // if (expr->getType()->isInt() && expr->getType()->getSize() == 32) {
        //     ImplicitCastExpr* temp = new ImplicitCastExpr(expr);
        //     this->expr = temp;
        // }
    } else if (op == UnaryExpr::SUB) {
        type = expr->getType();
        dst = new Operand(se);
        if (expr->isUnaryExpr()) {
            UnaryExpr* ue = (UnaryExpr*)expr;
            if (ue->getOp() == UnaryExpr::NOT)
                if (ue->getType() == TypeSystem::intType)
                    ue->setType(TypeSystem::boolType);
        }
    }
};

UnaryExpr::UnaryExpr(const UnaryExpr& u) : ExprNode(u) {
    op = u.op;
    expr = u.expr->copy();
    symbolEntry = new TemporarySymbolEntry(u.symbolEntry->getType(),
                                           SymbolTable::getLabel());
    dst = new Operand(symbolEntry);
}

void UnaryExpr::output(int level) {
    std::string op_str;
    switch (op) {
        case NOT:
            op_str = "not";
            break;
        case SUB:
            op_str = "minus";
            break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\ttype: %s\n", level, ' ',
            op_str.c_str(), type->toStr().c_str());
    expr->output(level + 4);
}

double UnaryExpr::getValue() {
    double value = 0;
    switch (op) {
        case NOT:
            value = !(expr->getValue());
            break;
        case SUB:
            value = -(expr->getValue());
            break;
    }
    return value;
}

void CallExpr::output(int level) {
    std::string name, type;
    int scope;
    if (symbolEntry) {
        name = symbolEntry->toStr();
        type = symbolEntry->getType()->toStr();
        scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
        fprintf(yyout, "%*cCallExpr\tfunction name: %s\tscope: %d\ttype: %s\n",
                level, ' ', name.c_str(), scope, type.c_str());
        Node* temp = param;
        while (temp) {
            temp->output(level + 4);
            temp = temp->getNext();
        }
    }
}

void Constant::output(int level) {
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    if (symbolEntry->getType()->isInt()) {
        fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
                value.c_str(), type.c_str());
    } else if (symbolEntry->getType()->isFloat()) {
        fprintf(yyout, "%*cFloatLiteral\tvalue: %s\ttype: %s\n", level, ' ',
                value.c_str(), type.c_str());
    }
}

double Constant::getValue() {
    // assert(symbolEntry->getType()->isInt());
    return ((ConstantSymbolEntry*)symbolEntry)->getValue();
}

double Id::getValue() {
    // assert(symbolEntry->getType()->isInt());
    return ((IdentifierSymbolEntry*)symbolEntry)->getValue();
}

void Id::output(int level) {
    std::string name, type;
    int scope;
    if (symbolEntry) {
        name = symbolEntry->toStr();
        type = symbolEntry->getType()->toStr();
        scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
        fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
                name.c_str(), scope, type.c_str());
        if (arrIdx) {
            ExprNode* temp = arrIdx;
            int i = 0;
            while (temp) {
                temp->output(level + 4 + 4 * i++);
                temp = (ExprNode*)(temp->getNext());
            }
        }
    }
}

void InitValueListExpr::output(int level) {
    std::string type;
    if (symbolEntry->getType())
        type = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cInitValueListExpr\ttype: %s\n", level, ' ',
            type.c_str());
    Node* temp = expr;
    while (temp) {
        temp->output(level + 4);
        temp = temp->getNext();
    }
}

void InitValueListExpr::addExpr(ExprNode* expr) {
    if (this->expr == nullptr) {
        assert(childCnt == 0);
        childCnt++;
        this->expr = expr;
    } else {
        childCnt++;
        this->expr->setNext(expr);
    }
}

bool InitValueListExpr::isFull() {
    ArrayType* type = (ArrayType*)(this->symbolEntry->getType());
    return childCnt == type->getLength();
}

void InitValueListExpr::fill() {
    if (allZero) {
        return;
    }
    Type* type = ((ArrayType*)(this->getType()))->getElementType();
    if (type->isArray()) {
        while (!isFull()) {
            this->addExpr(new InitValueListExpr(new ConstantSymbolEntry(type)));
        }
        ExprNode* temp = expr;
        while (temp) {
            ((InitValueListExpr*)temp)->fill();
            temp = (ExprNode*)(temp->getNext());
        }
    }
    if (type->isInt() || type->isFloat()) {
        while (!isFull()) {
            this->addExpr(new Constant(new ConstantSymbolEntry(type, 0)));
        }
        return;
    }
}

void ImplicitCastExpr::output(int level) {
    fprintf(yyout, "%*cImplicitCastExpr\ttype: %s to %s\n", level, ' ',
            expr->getType()->toStr().c_str(), type->toStr().c_str());
    this->expr->output(level + 4);
}

void CompoundStmt::output(int level) {
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    if (stmt)
        stmt->output(level + 4);
}

void SeqNode::output(int level) {
    // fprintf(yyout, "%*cSequence\n", level, ' ');
    stmt1->output(level);
    stmt2->output(level);
}

DeclStmt::DeclStmt(Id* id, ExprNode* expr) : id(id) {
    if (expr) {
        this->expr = expr;
        if (expr->isInitValueListExpr()) {
            ((InitValueListExpr*)(this->expr))->fill();
        } else {
            Type* idType = id->getType();
            Type* exprType = expr->getType();
            if ((idType->isFloat() && exprType->isInt()) ||
                (idType->isInt() && exprType->isFloat())) {
                ImplicitCastExpr* temp = new ImplicitCastExpr(expr, idType);
                this->expr = temp;
            }
        }
    }
}

void DeclStmt::output(int level) {
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    id->output(level + 4);
    if (expr)
        expr->output(level + 4);
    if (this->getNext()) {
        this->getNext()->output(level);
    }
}

void BlankStmt::output(int level) {
    fprintf(yyout, "%*cBlankStmt\n", level, ' ');
}

IfStmt::IfStmt(ExprNode* cond, StmtNode* thenStmt)
    : cond(cond), thenStmt(thenStmt) {
    if (cond->getType()->isInt() && cond->getType()->getSize() == 32) {
        ImplicitCastExpr* temp = new ImplicitCastExpr(cond);
        this->cond = temp;
    } else if (cond->getType()->isFloat()) {
        SymbolEntry* zero = new ConstantSymbolEntry(TypeSystem::floatType, 0);
        SymbolEntry* temp = new TemporarySymbolEntry(TypeSystem::boolType,
                                                     SymbolTable::getLabel());
        BinaryExpr* cmpZero = new BinaryExpr(temp, BinaryExpr::NOTEQUAL, cond,
                                             new Constant(zero));
        this->cond = cmpZero;
    }
};

void IfStmt::output(int level) {
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level) {
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void WhileStmt::output(int level) {
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    stmt->output(level + 4);
}
void BreakStmt::output(int level) {
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level) {
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void ReturnStmt::output(int level) {
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    if (retValue != nullptr)
        retValue->output(level + 4);
}

void AssignStmt::output(int level) {
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void ExprStmt::output(int level) {
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    expr->output(level + 4);
}

void FunctionDef::output(int level) {
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine\tfunction name: %s\ttype: %s\n", level,
            ' ', name.c_str(), type.c_str());
    if (decl) {
        decl->output(level + 4);
    }
    stmt->output(level + 4);
}

void ImplicitCastExpr::genCode() {
    expr->genCode();
    BasicBlock* bb = builder->getInsertBB();
    if (type == TypeSystem::boolType) {  // comparing ptr, should be ok here.
        Function* func = bb->getParent();
        BasicBlock* trueBB = new BasicBlock(func);
        BasicBlock* tempbb = new BasicBlock(func);
        BasicBlock* falseBB = new BasicBlock(func);

        new CmpInstruction(
            CmpInstruction::NE, this->dst, this->expr->getOperand(),
            new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), bb);
        this->trueList().push_back(
            new CondBrInstruction(trueBB, tempbb, this->dst, bb));
        this->falseList().push_back(new UncondBrInstruction(falseBB, tempbb));
    } else if (type->isInt()) {
        new FptosiInstruction(dst, this->expr->getOperand(), bb);
    } else if (type->isFloat()) {
        new SitofpInstruction(dst, this->expr->getOperand(), bb);
    } else {
        // error
        assert(false);
    }
}

double ImplicitCastExpr::getValue() {
    if (type == TypeSystem::boolType) {
        return -1;
    }
    double temp = expr->getValue();

    Type* srcType = expr->getType();

    if (type->isInt() && srcType->isFloat()) {
        float temp1 = (float)temp;
        int res = (int)temp1;
        temp = (double)res;
        return temp;
    } else if (type->isFloat() && srcType->isInt()) {
        int temp1 = (int)temp;
        float res = (float)temp1;
        temp = (double)res;
        return temp;
    } else {
        return -1;  // error
    }
}

ExprNode* ExprNode::copy() {
    ExprNode* ret;
    switch (kind) {
        case BINARYEXPR:
            ret = new BinaryExpr(*(BinaryExpr*)this);
            break;
        case UNARYEXPR:
            ret = new UnaryExpr(*(UnaryExpr*)this);
            break;
        case CALLEXPR:
            ret = new CallExpr(*(CallExpr*)this);
            break;
        case CONSTANT:
            ret = new Constant(*(Constant*)this);
            break;
        case ID:
            ret = new Id(*(Id*)this);
            break;
        case IMPLICITCASTEXPR:
            ret = new ImplicitCastExpr(*(ImplicitCastExpr*)this);
            break;
    }
    ExprNode* temp = this;
    if (temp->getNext()) {
        ret->cleanNext();
        temp = (ExprNode*)(temp->getNext());
        ret->setNext(temp->copy());
    }
    return ret;
}
