%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    #include <cstring>
    #include <stack>
    extern Ast ast;

    int yylex();
    int yyerror(char const*);
    ArrayType* arrayType;
    int idx;
    int* arrayValue;
    float* farrayValue;
    std::stack<InitValueListExpr*> stk;
    std::stack<StmtNode*> whileStk;
    InitValueListExpr* top;
    int leftCnt = 0;
    int whileCnt = 0;
    int paramNo = 0;
    extern int yylineno;
    #include <iostream>
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    float ftype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
    SymbolEntry* se;
}

%start Program
%token <strtype> ID STRING
%token <itype> INTEGER
%token <ftype> FLOATING
%token IF ELSE WHILE
%token INT VOID FLOAT // [ ] float
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON LBRACKET RBRACKET COMMA  
%token ADD SUB MUL DIV MOD OR AND LESS LESSEQUAL GREATER GREATEREQUAL ASSIGN EQUAL NOTEQUAL NOT
%token CONST
%token RETURN CONTINUE BREAK

%type<stmttype> Stmts Stmt AssignStmt ExprStmt BlockStmt IfStmt WhileStmt BreakStmt ContinueStmt ReturnStmt DeclStmt FuncDef ConstDeclStmt VarDeclStmt ConstDefList VarDef ConstDef VarDefList FuncFParam FuncFParams MaybeFuncFParams BlankStmt
%type<exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp MulExp ConstExp EqExp UnaryExp InitVal ConstInitVal InitValList ConstInitValList FuncArrayIndices FuncRParams ArrayIndices
%type<type> Type

%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt
    : AssignStmt {
        $$=$1;
    }
    | ExprStmt {$$ = $1;}
    | BlockStmt {$$=$1;}
    | BlankStmt {$$ = $1;}
    | IfStmt {$$ = $1;}
    | WhileStmt {$$ = $1;}
    | BreakStmt {
        if (!whileCnt)
            fprintf(stderr, "\'break\' statement not in while statement\n");
        $$=$1;
    }
    | ContinueStmt {
        if (!whileCnt)
            fprintf(stderr, "\'continue\' statement not in while statement\n");
        $$=$1;
    }
    | ReturnStmt {$$ = $1;}
    | DeclStmt {$$ = $1;}
    | FuncDef {$$ = $1;}
    ;
LVal
    : ID {
        SymbolEntry* se;
        se = identifiers->lookup($1);
        if (se == nullptr)
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
        $$ = new Id(se);
        delete []$1;
    }
    | ID ArrayIndices{
        SymbolEntry* se;
        se = identifiers->lookup($1);
        if (se == nullptr)
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
        $$ = new Id(se, $2);
        delete []$1;
    }
    ; 
AssignStmt
    : LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;
ExprStmt
    : Exp SEMICOLON {
        $$ = new ExprStmt($1);
    }
    ;
BlankStmt
    : SEMICOLON {
        $$ = new BlankStmt();
    }
    ;
BlockStmt
    : LBRACE {
        identifiers = new SymbolTable(identifiers);
    } 
      Stmts RBRACE {
        // midrule actions https://www.gnu.org/software/bison/manual/html_node/Using-Midrule-Actions.html
        $$ = new CompoundStmt($3);

        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | LBRACE RBRACE {
        // 这里这个用加嘛 不确定
        $$ = new CompoundStmt();
    }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;
WhileStmt
    : WHILE LPAREN Cond RPAREN {
        whileCnt++;
        WhileStmt *whileNode = new WhileStmt($3);
        $<stmttype>$ = whileNode;
        whileStk.push(whileNode);
    }
    Stmt {
        StmtNode *whileNode = $<stmttype>5; 
        ((WhileStmt*)whileNode)->setStmt($6);
        $$=whileNode;
        whileStk.pop();
        whileCnt--;
    }
    ;
BreakStmt
    : BREAK SEMICOLON {
        $$ = new BreakStmt(whileStk.top());
    }
    ;
ContinueStmt
    : CONTINUE SEMICOLON {
        $$ = new ContinueStmt(whileStk.top());
    }
    ;
ReturnStmt
    : RETURN SEMICOLON {
        $$ = new ReturnStmt();
    }
    | RETURN Exp SEMICOLON {
        $$ = new ReturnStmt($2);
    }
    ;
Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;
PrimaryExp
    : LPAREN Exp RPAREN {
        $$ = $2;
    }
    | LVal {
        $$ = $1;
    }
    | STRING {
        SymbolEntry* se;
        se = globals->lookup(std::string($1));
        // 这里如果str内容和变量名相同 怎么处理
        if (se == nullptr){
            Type* type = new StringType(strlen($1));
            se = new ConstantSymbolEntry(type, std::string($1));
            globals->install(std::string($1), se);
        }
        ExprNode* expr = new ExprNode(se);

        $$ = expr;
    }
    | INTEGER {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    | FLOATING {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::floatType, $1);
        $$ = new Constant(se);
        // [ ] ast for floating number
    }
    ;
UnaryExp 
    : PrimaryExp {$$ = $1;}
    | ID LPAREN FuncRParams RPAREN {
        SymbolEntry* se;
        se = identifiers->lookup($1);
        if (se == nullptr)
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)$1);
        $$ = new CallExpr(se, $3);
    }
    | ID LPAREN RPAREN {
        SymbolEntry* se;
        se = identifiers->lookup($1);
        if (se == nullptr)
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)$1);
        if (strcmp($1, "_sysy_starttime") == 0 || strcmp($1, "_sysy_stoptime") == 0) {
            ExprNode* param = new Constant(new ConstantSymbolEntry(TypeSystem::intType, yylineno));
            $$ = new CallExpr(se, param);
        } else
            $$ = new CallExpr(se);
    }
    | ADD UnaryExp {$$ = $2;}
    | SUB UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2); // [ ] float
    }
    | NOT UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    ;
MulExp
    : UnaryExp {$$ = $1;}
    | MulExp MUL UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3); // [ ] float
    }
    | MulExp DIV UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3); // [ ] float
    }
    | MulExp MOD UnaryExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3); // [ ] float
    }
    ;
AddExp
    : MulExp {$$ = $1;}
    | AddExp ADD MulExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3); // [ ] float
    }
    | AddExp SUB MulExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3); // [ ] float
    }
    ;
RelExp
    : AddExp {
        $$ = $1;
    }
    | RelExp LESS AddExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    | RelExp LESSEQUAL AddExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    | RelExp GREATER AddExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    | RelExp GREATEREQUAL AddExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, $1, $3);
    }
    ;
EqExp
    : RelExp {$$ = $1;}
    | EqExp EQUAL RelExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    | EqExp NOTEQUAL RelExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    }
    ;
LAndExp
    : EqExp {$$ = $1;}
    | LAndExp AND EqExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    : LAndExp {$$ = $1;}
    | LOrExp OR LAndExp {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
ConstExp
    : AddExp {$$ = $1;}
    ;
FuncRParams 
    : Exp {$$ = $1;}
    | FuncRParams COMMA Exp {
        $$ = $1;
        $$->setNext($3);
    }
Type
    : INT {
        $$ = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
    | FLOAT {
        $$ = TypeSystem::floatType;
    }
    ;
DeclStmt
    : VarDeclStmt {$$ = $1;}
    | ConstDeclStmt {$$ = $1;}
    ;
VarDeclStmt
    : Type VarDefList SEMICOLON {$$ = $2;}
    ;
ConstDeclStmt
    : CONST Type ConstDefList SEMICOLON {
        // 这里肯定还得区分一下 
        $$ = $3;
    }
    ;
VarDefList
    : VarDefList COMMA VarDef {
        $$ = $1;
        $1->setNext($3);
    } 
    | VarDef {$$ = $1;}
    ;
ConstDefList
    : ConstDefList COMMA ConstDef {
        $$ = $1;
        $1->setNext($3);
    }
    | ConstDef {$$ = $1;}
    ;
VarDef
    : ID {
        SymbolEntry* se; // [ ] float
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        if (!identifiers->install($1, se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$1);
        $$ = new DeclStmt(new Id(se));
        delete []$1;
    }
    | ID ArrayIndices {
        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = $2;
        while(temp){
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type *type = TypeSystem::intType; // [ ] float 
        Type* temp1;
        while(!vec.empty()){
            temp1 = new ArrayType(type, vec.back());
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
            vec.pop_back();
        }
        arrayType = (ArrayType*)type;
        se = new IdentifierSymbolEntry(type, $1, identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setAllZero();
        // int *p = new int[type->getSize()];
        // ((IdentifierSymbolEntry*)se)->setArrayValue(p);
        if (!identifiers->install($1, se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$1);
        $$ = new DeclStmt(new Id(se));
        delete []$1;
    }
    | ID ASSIGN InitVal {
        SymbolEntry* se; // [ ] float
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        if (!identifiers->install($1, se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$1);
        // 这里要不要存值不确定
        ((IdentifierSymbolEntry*)se)->setValue($3->getValue());
        $$ = new DeclStmt(new Id(se), $3);
        delete []$1;
    }
    | ID ArrayIndices ASSIGN {
        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = $2;
        while(temp){
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type* type = TypeSystem::intType; // [ ] float
        Type* temp1;
        for(auto it = vec.rbegin(); it != vec.rend(); it++) {
            temp1 = new ArrayType(type, *it);
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
        }
        arrayType = (ArrayType*)type;
        idx = 0;
        std::stack<InitValueListExpr*>().swap(stk);
        se = new IdentifierSymbolEntry(type, $1, identifiers->getLevel());
        $<se>$ = se;
        arrayValue = new int[arrayType->getSize()];
    }
      InitVal {
        ((IdentifierSymbolEntry*)$<se>4)->setArrayValue(arrayValue);
        if (((InitValueListExpr*)$5)->isEmpty())
            ((IdentifierSymbolEntry*)$<se>4)->setAllZero();
        if (!identifiers->install($1, $<se>4))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$1);
        $$ = new DeclStmt(new Id($<se>4), $5);
        delete []$1;
    }
    ;
ConstDef
    : ID ASSIGN ConstInitVal {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(TypeSystem::constIntType, $1, identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setConst();
        if (!identifiers->install($1, se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$1);
        identifiers->install($1, se);
        ((IdentifierSymbolEntry*)se)->setValue($3->getValue());
        $$ = new DeclStmt(new Id(se), $3);
        delete []$1;
    }
    | ID ArrayIndices ASSIGN  {
        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = $2;
        while(temp){
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type* type = TypeSystem::constIntType; // [ ] float
        Type* temp1;
        for(auto it = vec.rbegin(); it != vec.rend(); it++) {
            temp1 = new ArrayType(type, *it, true);
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
        }
        arrayType = (ArrayType*)type;
        idx = 0;
        std::stack<InitValueListExpr*>().swap(stk);
        se = new IdentifierSymbolEntry(type, $1, identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setConst();
        $<se>$ = se;
        arrayValue = new int[arrayType->getSize()];
    }
      ConstInitVal {
        ((IdentifierSymbolEntry*)$<se>4)->setArrayValue(arrayValue);
        if (!identifiers->install($1, $<se>4))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$1);
        identifiers->install($1, $<se>4);
        $$ = new DeclStmt(new Id($<se>4), $5);
        delete []$1;
    } 
    ;
ArrayIndices
    : LBRACKET ConstExp RBRACKET {
        $$ = $2;
    }
    | ArrayIndices LBRACKET ConstExp RBRACKET {
        $$ = $1;
        $1->setNext($3);
    }
    ;
InitVal 
    : Exp {
        if (!$1->getType()->isInt()){
            fprintf(stderr,
                "cannot initialize a variable of type \'int\' with an rvalue "
                "of type \'%s\'\n",
                $1->getType()->toStr().c_str());
        } // [ ] float 
        $$ = $1;
        if (!stk.empty()){
            arrayValue[idx++] = $1->getValue();
            Type* arrTy = stk.top()->getSymbolEntry()->getType();
            if (arrTy == TypeSystem::intType) // [ ] float
                stk.top()->addExpr($1);
            else
                while(arrTy){
                    if (((ArrayType*)arrTy)->getElementType() != TypeSystem::intType){
                        arrTy = ((ArrayType*)arrTy)->getElementType();
                        SymbolEntry* se = new ConstantSymbolEntry(arrTy);
                        InitValueListExpr* list = new InitValueListExpr(se);
                        stk.top()->addExpr(list);
                        stk.push(list);
                    }else{
                        stk.top()->addExpr($1);
                        while(stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt){
                            arrTy = ((ArrayType*)arrTy)->getArrayType();
                            stk.pop();
                        }
                        break;
                    }
                }
        }         
    }
    | LBRACE RBRACE {
        SymbolEntry* se;
        ExprNode* list;
        if (stk.empty()){
            // 如果只用一个{}初始化数组，那么栈一定为空
            // 此时也没必要再加入栈了
            memset(arrayValue, 0, arrayType->getSize());
            idx += arrayType->getSize() / TypeSystem::intType->getSize(); // [ ] float
            se = new ConstantSymbolEntry(arrayType);
            list = new InitValueListExpr(se);
        }else{
            // 栈不空说明肯定不是只有{}
            // 此时需要确定{}到底占了几个元素
            Type* type = ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType();
            int len = type->getSize() / TypeSystem::intType->getSize(); // [ ] float
            memset(arrayValue + idx, 0, type->getSize());
            idx += len;
            se = new ConstantSymbolEntry(type);
            list = new InitValueListExpr(se);
            stk.top()->addExpr(list);
            while(stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt){
                stk.pop();
            }
        }
        $$ = list;
    }
    | LBRACE {
        SymbolEntry* se;
        if (!stk.empty())
            arrayType = (ArrayType*)(((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
        se = new ConstantSymbolEntry(arrayType);
        if (arrayType->getElementType() != TypeSystem::intType){
            arrayType = (ArrayType*)(arrayType->getElementType());
        } // [ ] float
        InitValueListExpr* expr = new InitValueListExpr(se);
        if (!stk.empty())
            stk.top()->addExpr(expr);
        stk.push(expr);
        $<exprtype>$ = expr;
        leftCnt++;
    } 
      InitValList RBRACE {
        leftCnt--;
        while(stk.top() != $<exprtype>2 && stk.size() > (long unsigned int)(leftCnt + 1))
            stk.pop();
        if (stk.top() == $<exprtype>2)
            stk.pop();
        $$ = $<exprtype>2;
        if (!stk.empty())
            while(stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt){
                stk.pop();
            }
        int size = ((ArrayType*)($$->getSymbolEntry()->getType()))->getSize()/ TypeSystem::intType->getSize();
        while(idx % size != 0)
            arrayValue[idx++] = 0;
        if (!stk.empty())
            arrayType = (ArrayType*)(((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
    }
    ;

ConstInitVal
    : ConstExp {
        $$ = $1;
        if (!stk.empty()){
            arrayValue[idx++] = $1->getValue();
            Type* arrTy = stk.top()->getSymbolEntry()->getType();
            if (arrTy == TypeSystem::constIntType)
                stk.top()->addExpr($1); // [ ] float
            else
                while(arrTy){
                    if (((ArrayType*)arrTy)->getElementType() != TypeSystem::constIntType){ // [ ] float
                        arrTy = ((ArrayType*)arrTy)->getElementType();
                        SymbolEntry* se = new ConstantSymbolEntry(arrTy);
                        InitValueListExpr* list = new InitValueListExpr(se);
                        stk.top()->addExpr(list);
                        stk.push(list);
                    }else{
                        stk.top()->addExpr($1);
                        while(stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt){
                            arrTy = ((ArrayType*)arrTy)->getArrayType();
                            stk.pop();
                        }
                        break;
                    }
                }
        }
    }
    | LBRACE RBRACE {
        SymbolEntry* se;
        ExprNode* list;
        if (stk.empty()){
            // 如果只用一个{}初始化数组，那么栈一定为空
            // 此时也没必要再加入栈了
            memset(arrayValue, 0, arrayType->getSize());
            idx += arrayType->getSize() / TypeSystem::constIntType->getSize();
            se = new ConstantSymbolEntry(arrayType); // [ ] float
            list = new InitValueListExpr(se);
        }else{
            // 栈不空说明肯定不是只有{}
            // 此时需要确定{}到底占了几个元素
            Type* type = ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType();
            int len = type->getSize() / TypeSystem::constIntType->getSize();
            memset(arrayValue + idx, 0, type->getSize());
            idx += len;
            se = new ConstantSymbolEntry(type);
            list = new InitValueListExpr(se);
            stk.top()->addExpr(list);
            while(stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt){
                stk.pop();
            }
        }
        $$ = list;
    }
    | LBRACE {
        SymbolEntry* se;
        if (!stk.empty())
            arrayType = (ArrayType*)(((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
        se = new ConstantSymbolEntry(arrayType);
        if (arrayType->getElementType() != TypeSystem::intType){ // [ ] float
            arrayType = (ArrayType*)(arrayType->getElementType());
        }
        InitValueListExpr* expr = new InitValueListExpr(se);
        if (!stk.empty())
            stk.top()->addExpr(expr);
        stk.push(expr);
        $<exprtype>$ = expr;
        leftCnt++;
    } 
      ConstInitValList RBRACE {
        leftCnt--;
        while(stk.top() != $<exprtype>2 && stk.size() > (long unsigned int)(leftCnt + 1))
            stk.pop();
        if (stk.top() == $<exprtype>2)
            stk.pop();
        $$ = $<exprtype>2;
        if (!stk.empty())
            while(stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt){
                stk.pop();
            }
        while(idx % (((ArrayType*)($$->getSymbolEntry()->getType()))->getSize()/ sizeof(int)) !=0 )
            arrayValue[idx++] = 0;
        if (!stk.empty())
            arrayType = (ArrayType*)(((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
    }
    ;
InitValList
    : InitVal {
        $$ = $1;
    }
    | InitValList COMMA InitVal {
        $$ = $1;
    }
    ;
ConstInitValList
    : ConstInitVal {
        $$ = $1;
    }
    | ConstInitValList COMMA ConstInitVal {
        $$ = $1;
    }
    ;
FuncDef
    :
    Type ID {
        // SymbolTable::resetLabel();
        identifiers = new SymbolTable(identifiers);
        paramNo = 0;
    }
    LPAREN MaybeFuncFParams RPAREN {
        Type* funcType;
        std::vector<Type*> vec;
        std::vector<SymbolEntry*> vec1;
        DeclStmt* temp = (DeclStmt*)$5;
        while(temp){
            vec.push_back(temp->getId()->getSymbolEntry()->getType());
            vec1.push_back(temp->getId()->getSymbolEntry());
            temp = (DeclStmt*)(temp->getNext());
        }
        funcType = new FunctionType($1, vec, vec1);
        SymbolEntry* se = new IdentifierSymbolEntry(funcType, $2, identifiers->getPrev()->getLevel());
        if (!identifiers->getPrev()->install($2, se)){
            fprintf(stderr, "redefinition of \'%s %s\'\n", $2, se->getType()->toStr().c_str());
        }
        $<se>$ = se; 
    } 
    BlockStmt {
        $$ = new FunctionDef($<se>7, (DeclStmt*)$5, $8);
        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;
MaybeFuncFParams
    : FuncFParams {$$ = $1;}
    | %empty {$$ = nullptr;}
FuncFParams
    : FuncFParams COMMA FuncFParam {
        $$ = $1;
        $$->setNext($3);
    }
    | FuncFParam {
        $$ = $1;
    }
    ;
FuncFParam
    : Type ID {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel(), paramNo++);
        identifiers->install($2, se);
        ((IdentifierSymbolEntry*)se)->setLabel();
        ((IdentifierSymbolEntry*)se)->setAddr(new Operand(se));
        $$ = new DeclStmt(new Id(se));
        delete []$2;
    }
    | Type ID FuncArrayIndices {
        // 这里也需要求值
        SymbolEntry* se;
        ExprNode* temp = $3;
        Type* arr = TypeSystem::intType; // [ ] float
        Type* arr1;
        std::stack<ExprNode*> stk;
        while(temp){
            stk.push(temp);
            temp = (ExprNode*)(temp->getNext());
        }
        while(!stk.empty()){
            arr1 = new ArrayType(arr, stk.top()->getValue());
            if (arr->isArray())
                ((ArrayType*)arr)->setArrayType(arr1);
            arr = arr1;
            stk.pop();
        }
        se = new IdentifierSymbolEntry(arr, $2, identifiers->getLevel(), paramNo++);
        identifiers->install($2, se);
        ((IdentifierSymbolEntry*)se)->setLabel();
        ((IdentifierSymbolEntry*)se)->setAddr(new Operand(se));
        $$ = new DeclStmt(new Id(se));
        delete []$2;
    }
    ;
FuncArrayIndices 
    : LBRACKET RBRACKET {
        $$ = new ExprNode(nullptr);
    }
    | FuncArrayIndices LBRACKET Exp RBRACKET {
        $$ = $1;
        $$->setNext($3);
    }
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
