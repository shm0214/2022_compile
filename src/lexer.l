%option noyywrap
%{
    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include "parser.h"
    #include <ostream>
    #include <fstream>
    #include <iostream>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;
    extern bool dump_tokens;
    int chars = 0;
    void DUMP_TOKEN(std::string s){
        std::string DEBUG_INFO = "[TOKEN]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
    #include <iostream>
%}
DECIMIAL ([1-9][0-9]*|0)
OCTAL (0[0-7]+)
HEXADECIMAL (0[xX][0-9A-Fa-f]+)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
STRING \".*\"
EOL (\r\n|\n|\r)
WHITE [\t ]
BLOCKCOMMENTBEGIN \/\*
BLOCKCOMMENTELEMENT .
BLOCKCOMMENTEND \*\/
%x BLOCKCOMMENT
LINECOMMENT \/\/[^\n]*

DECIMAL_FLOAT ((([0-9]*[.][0-9]*([eE][+-]?[0-9]+)?)|([0-9]+[eE][+-]?[0-9]+))[fLlL]?)
HEXADECIMAL_FLOAT (0[xX](([0-9A-Fa-f]*[.][0-9A-Fa-f]*([pP][+-]?[0-9]+)?)|([0-9A-Fa-f]+[pP][+-]?[0-9]+))[fLlL]?)

%%
"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    if(dump_tokens)
        DUMP_TOKEN("INT\tint");
    chars += strlen("int");
    return INT;
}
"float" {
    if(dump_tokens)
        DUMP_TOKEN("FLOAT\tfloat");
    chars += strlen("float");
    return FLOAT;

}
"void" {
    if(dump_tokens)
        DUMP_TOKEN("VOID\tvoid");
    chars += strlen("void");
    return VOID;
}
"const" {
    if(dump_tokens)
        DUMP_TOKEN("CONST\tconst");
    chars += strlen("const");
    return CONST;
}
"if" {
    if(dump_tokens)
        DUMP_TOKEN("IF\tif");
    chars += strlen("if");
    return IF;
};
"else" {
    if(dump_tokens)
        DUMP_TOKEN("ELSE\telse");
    chars += strlen("else");
    return ELSE;
};
"return" {
    if(dump_tokens)
        DUMP_TOKEN("RETURN\treturn");
    chars += strlen("return");
    return RETURN;
}
"while" {
    if(dump_tokens)
        DUMP_TOKEN("WHILE\twhile");
    chars += strlen("while");
    return WHILE;
}
"break" {
    if(dump_tokens)
        DUMP_TOKEN("BREAK\tbreak");
    chars += strlen("break");
    return BREAK;
}
"continue" {
    if(dump_tokens)
        DUMP_TOKEN("CONTINUE\tcontinue");
    chars += strlen("continue");
    return CONTINUE;
}
"==" {
    if(dump_tokens)
        DUMP_TOKEN("EQUAL\t==");
    chars += strlen("==");
    return EQUAL;
}
"!=" {
    if(dump_tokens)
        DUMP_TOKEN("NOTEQUAL\t!=");
    chars += strlen("!=");
    return NOTEQUAL;
}
"=" {
    if(dump_tokens)
        DUMP_TOKEN("ASSIGN\t=");
    chars += strlen("=");
    return ASSIGN;
}
"<" {
    if(dump_tokens)
        DUMP_TOKEN("LESS\t<");
    chars += strlen("<");
    return LESS;
}
"<=" {
    if(dump_tokens)
        DUMP_TOKEN("LESSEQUAL\t<");
    chars += strlen("<=");
    return LESSEQUAL;
}
">" {
    if(dump_tokens)
        DUMP_TOKEN("GREATER\t<");
    chars += strlen(">");
    return GREATER;
}
">=" {
    if(dump_tokens)
        DUMP_TOKEN("GREATEREQUAL\t<");
    chars += strlen(">=");
    return GREATEREQUAL;
}
"+" {
    if(dump_tokens)
        DUMP_TOKEN("ADD\t+");
    chars += strlen("+");
    return ADD;
}
"-" {
    if(dump_tokens)
        DUMP_TOKEN("SUB\t-");
    chars += strlen("-");
    return SUB;
}
"*" {
    if(dump_tokens)
        DUMP_TOKEN("MUL\t*");
    chars += strlen("*");
    return MUL;
}
"/" {
    if(dump_tokens)
        DUMP_TOKEN("DIV\t/");
    chars += strlen("/");
    return DIV;
}
"%" {
    if(dump_tokens)
        DUMP_TOKEN("MOD\t%");
    chars += strlen("%");
    return MOD;
}
"&&" {
    if(dump_tokens)
        DUMP_TOKEN("AND\t&&");
    chars += strlen("&&");
    return AND;
}
"||" {
    if(dump_tokens)
        DUMP_TOKEN("OR\t||");
    chars += strlen("||");
    return OR;
}
"!" {
    if(dump_tokens)
        DUMP_TOKEN("NOT\t!");
    chars += strlen("!");
    return NOT;
}
";" {
    if(dump_tokens)
        DUMP_TOKEN("SEMICOLON\t;");
    chars += strlen(";");
    return SEMICOLON;
}
"(" {
    if(dump_tokens)
        DUMP_TOKEN("LPAREN\t(");
    chars += strlen("(");
    return LPAREN;
}
")" {
    if(dump_tokens)
        DUMP_TOKEN("RPAREN\t)");
    chars += strlen(")");
    return RPAREN;
}
"{" {
    if(dump_tokens)
        DUMP_TOKEN("LBRACE\t{");
    chars += strlen("{");
    return LBRACE;
}
"}" {
    if(dump_tokens)
        DUMP_TOKEN("RBRACE\t}");
    chars += strlen("}");
    return RBRACE;
}
"[" {
    if(dump_tokens)
        DUMP_TOKEN("LBRACKET\t[");
    chars += strlen("[");
    return LBRACKET;
}
"]" {
    if(dump_tokens)
        DUMP_TOKEN("RBRACKET\t]");
    chars += strlen("]");
    return RBRACKET;
}
"," {
    if(dump_tokens)
        DUMP_TOKEN("COMMA\t,");
    chars += strlen(",");
    return COMMA;
}
{DECIMIAL} {
    if(dump_tokens)
        DUMP_TOKEN(string("NUMBER\t") + string(yytext));
    chars += strlen(yytext);
    yylval.numtype = std::stol(yytext);
    return INTEGER;
}
{OCTAL} {
    int temp;
    sscanf(yytext, "%o", &temp);
    if(dump_tokens)
        DUMP_TOKEN(string("NUMBER\t") + to_string(temp));
    yylval.numtype = temp;
    return INTEGER;
}
{HEXADECIMAL} {
    int temp;
    sscanf(yytext, "%x", &temp);
    if(dump_tokens)
        DUMP_TOKEN(string("NUMBER\t") + to_string(temp));
    yylval.numtype = temp;
    return INTEGER;
}

{DECIMAL_FLOAT} {
    float temp;
    sscanf(yytext, "%f", &temp);
    if (dump_tokens)
        DUMP_TOKEN(string("NUMBER\t") + to_string(temp));
    // chars += strlen(yytext);
    yylval.numtype = (double)temp;
    return FLOATING;
}

{HEXADECIMAL_FLOAT} {
    float temp;
    sscanf(yytext, "%f", &temp);
    if (dump_tokens)
        DUMP_TOKEN(string("NUMBER\t") + to_string(temp));
    // chars += strlen(yytext);
    yylval.numtype = (double)temp;
    return FLOATING;
}

"putint" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    vec.push_back(TypeSystem::intType);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(1);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}

"getint" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    Type* funcType = new FunctionType(TypeSystem::intType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(0);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}
"putch" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    vec.push_back(TypeSystem::intType);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(1);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}
"getch" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    Type* funcType = new FunctionType(TypeSystem::intType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(0);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}

"putfloat" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    vec.push_back(TypeSystem::floatType);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(0);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(1);
    st->install(yytext, se);
    return ID;
}

"getfloat" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    Type* funcType = new FunctionType(TypeSystem::floatType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(0);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}
"putarray" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    vec.push_back(TypeSystem::intType);
    ArrayType* arr = new ArrayType(TypeSystem::intType, -1);
    vec.push_back(arr);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(2);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}

"getarray" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    ArrayType* arr = new ArrayType(TypeSystem::intType, -1);
    vec.push_back(arr);
    Type* funcType = new FunctionType(TypeSystem::intType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(1);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}
"putfarray" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    vec.push_back(TypeSystem::intType); // param 1 is int
    ArrayType* arr = new ArrayType(TypeSystem::floatType, -1);
    vec.push_back(arr);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(2);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}

"getfarray" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    ArrayType* arr = new ArrayType(TypeSystem::floatType, -1);
    vec.push_back(arr);
    // return length so shall be int
    Type* funcType = new FunctionType(TypeSystem::intType, vec, vec1); 
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(1);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(yytext, se);
    return ID;
}

"putf" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    StringType* str = new StringType(0);
    vec.push_back(str);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    st->install(yytext, se);
    return ID;
}
"starttime" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[6 + strlen(yytext) + 1];
    char* sysy = (char *)"_sysy_";
    strcpy(lexeme, sysy);
    strcpy(lexeme + 6, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    vec.push_back(TypeSystem::intType);
    std::vector<SymbolEntry*> vec1;
    Type* funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    char *temp = new char[6+strlen(yytext)];
    memcpy(temp, lexeme, 6+strlen(yytext));
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, temp, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(1);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(temp, se);
    return ID;
}
"stoptime" {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[6 + strlen(yytext) + 1];
    char* sysy = (char*)"_sysy_";
    strcpy(lexeme, sysy);
    strcpy(lexeme + 6, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    vec.push_back(TypeSystem::intType);
    std::vector<SymbolEntry*> vec1;
    Type* funcType = new FunctionType(TypeSystem::voidType, vec, vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    char *temp = new char[6+strlen(yytext)];
    memcpy(temp, lexeme, 14);
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, temp, st->getLevel(), -1, true);
    ((IdentifierSymbolEntry*)se)->setIntParamNo(1);
    ((IdentifierSymbolEntry*)se)->setFloatParamNo(0);
    st->install(temp, se);
    return ID;
}

{ID} {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}
{STRING} {
    if(dump_tokens)
        DUMP_TOKEN(yytext);
    char* lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return STRING;
}
<*>{EOL} {
    chars += strlen(yytext);
    yylineno++;
    chars = 0;
}
{WHITE}
{LINECOMMENT} {}
{BLOCKCOMMENTBEGIN} {BEGIN BLOCKCOMMENT;}
<BLOCKCOMMENT>{BLOCKCOMMENTELEMENT} {}
<BLOCKCOMMENT>{BLOCKCOMMENTEND} {BEGIN INITIAL;}


%%
