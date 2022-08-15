#include "Type.h"
#include <assert.h>
#include <sstream>

// 不确定这个size改动会不会导致数组出问题，感觉应该没事

IntType TypeSystem::commonConstInt = IntType(32, true);
IntType TypeSystem::commonInt = IntType(32);
IntType TypeSystem::commonInt8 = IntType(8);
IntType TypeSystem::commonBool = IntType(1);
VoidType TypeSystem::commonVoid = VoidType();
FloatType TypeSystem::commonFloat = FloatType(32);
FloatType TypeSystem::commonConstFloat = FloatType(32, true);

Type* TypeSystem::constIntType = &commonConstInt;
Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::int8Type = &commonInt8;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;
Type* TypeSystem::floatType = &commonFloat;
Type* TypeSystem::constFloatType = &commonConstFloat;

std::string IntType::toStr() {
    std::ostringstream buffer;
    if (constant)
        buffer << "i";
    else
        buffer << "i";
    buffer << size;
    return buffer.str();
}

std::string FloatType::toStr() {
    return "float";
}

std::string VoidType::toStr() {
    return "void";
}

std::string ArrayType::toStr() {
    std::vector<std::string> vec;
    Type* temp = this;
    int count = 0;
    bool flag = false;
    while (temp && temp->isArray()) {
        std::ostringstream buffer;
        if (((ArrayType*)temp)->getLength() == -1) {
            flag = true;
        } else {
            buffer << "[" << ((ArrayType*)temp)->getLength() << " x ";
            count++;
            vec.push_back(buffer.str());
        }
        temp = ((ArrayType*)temp)->getElementType();
    }
    std::ostringstream buffer;
    for (auto it = vec.begin(); it != vec.end(); it++)
        buffer << *it;
    if (temp->isInt()) {
        buffer << "i32";
    } else if (temp->isFloat()) {
        buffer << "float";
    } else {
        assert(false);  // invalid type
    }
    while (count--)
        buffer << ']';
    if (flag)
        buffer << '*';
    return buffer.str();
}

std::string FunctionType::toStr() {
    std::ostringstream buffer;
    buffer << returnType->toStr() << "(";
    for (auto it = paramsType.begin(); it != paramsType.end(); it++) {
        buffer << (*it)->toStr();
        if (it + 1 != paramsType.end())
            buffer << ", ";
    }
    buffer << ')';
    return buffer.str();
}

std::string StringType::toStr() {
    std::ostringstream buffer;
    buffer << "const char[" << length << "]";
    return buffer.str();
}

std::string PointerType::toStr() {
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}

bool Type::isPtr2Array() {
    if (isPtr())
        return ((PointerType*)this)->getType()->isArray();
    return false;
}