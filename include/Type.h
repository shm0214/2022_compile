#ifndef __TYPE_H__
#define __TYPE_H__
#include <string>
#include <vector>
#include "SymbolTable.h"

class Type {
   private:
    int kind;

   protected:
    enum { INT, VOID, FLOAT, FUNC, PTR, ARRAY, STRING };
    long long size;

   public:
    Type(int kind, long long size = 0) : kind(kind), size(size){};
    virtual ~Type(){};
    virtual std::string toStr() = 0;
    bool isInt() const { return kind == INT; };
    bool isFloat() const { return kind == FLOAT; };
    bool isVoid() const { return kind == VOID; };
    bool isFunc() const { return kind == FUNC; };
    bool isPtr() const { return kind == PTR; };
    bool isArray() const { return kind == ARRAY; };
    bool isString() const { return kind == STRING; };
    int getKind() const { return kind; };
    bool isPtr2Array();
    long long getSize() const { return size; };
};

class IntType : public Type {
   private:
    bool constant;

   public:
    IntType(int size, bool constant = false)
        : Type(Type::INT, size), constant(constant){};
    std::string toStr();
    bool isConst() const { return constant; };
};

class FloatType : public Type {
   private:
    bool constant;

   public:
    FloatType(int size, bool constant = false)
        : Type(Type::FLOAT, size), constant(constant){};

    std::string toStr();
    bool isConst() const { return constant; };
};

class VoidType : public Type {
   public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type {
   private:
    Type* returnType;
    std::vector<Type*> paramsType;
    std::vector<SymbolEntry*> paramsSe;

   public:
    FunctionType(Type* returnType,
                 std::vector<Type*> paramsType,
                 std::vector<SymbolEntry*> paramsSe)
        : Type(Type::FUNC),
          returnType(returnType),
          paramsType(paramsType),
          paramsSe(paramsSe){};
    void setParamsType(std::vector<Type*> paramsType) {
        this->paramsType = paramsType;
    };
    std::vector<Type*> getParamsType() { return paramsType; };
    std::vector<SymbolEntry*> getParamsSe() { return paramsSe; };
    Type* getRetType() { return returnType; };
    std::string toStr();
};

class ArrayType : public Type {
   private:
    Type* elementType;
    Type* arrayType = nullptr;
    int length;
    bool constant;

   public:
    ArrayType(Type* elementType, int length, bool constant = false)
        : Type(Type::ARRAY),
          elementType(elementType),
          length(length),
          constant(constant) {
        size = elementType->getSize() * length;
    };
    std::string toStr();
    int getLength() const { return length; };
    Type* getElementType() const { return elementType; };
    void setArrayType(Type* arrayType) { this->arrayType = arrayType; };
    bool isConst() const { return constant; };
    Type* getArrayType() const { return arrayType; };
};

class StringType : public Type {
   private:
    int length;

   public:
    StringType(int length) : Type(Type::STRING), length(length){};
    int getLength() const { return length; };
    std::string toStr();
};

class PointerType : public Type {
   private:
    Type* valueType;

   public:
    PointerType(Type* valueType) : Type(Type::PTR) {
        this->valueType = valueType;
    };
    std::string toStr();
    Type* getType() const { return valueType; };
};

class TypeSystem {
   private:
    static IntType commonInt;
    static IntType commonBool;
    static IntType commonInt8;
    static FloatType commonFloat;
    static VoidType commonVoid;
    static IntType commonConstInt;
    static FloatType commonConstFloat;

   public:
    static Type* intType;
    static Type* int8Type;
    static Type* floatType;
    static Type* voidType;
    static Type* boolType;
    static Type* constIntType;
    static Type* constFloatType;
};

#endif
