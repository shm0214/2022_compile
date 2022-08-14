#ifndef __SSA_GRAPH_H__
#define __SSA_GRAPH_H__
#include <vector>

class Instruction;

class SSAGraphNode {
    // CONST 没有
    Instruction* ins;
    // only for CONST
    int val;
    int type;
    std::vector<SSAGraphNode*> children;

   public:
    enum {
        CONST,
        ALLOCA,
        SUB,
        ADD,
        AND,
        OR,
        MUL,
        DIV,
        MOD,
        E,
        NE,
        L,
        LE,
        G,
        GE,
        PHI,
        CALL,
        LOAD,
        GEP,
        ZEXT,
        XOR,
        BITCAST,
        SHL,
        ASHR,
    };
    SSAGraphNode(Instruction* ins, int type = -1) : ins(ins), type(type) {}
<<<<<<< HEAD
    SSAGraphNode(int val) : type(CONST) {}
    Instruction* getInst() { return ins; }
    void setNo(int num) { no = num; }
    int getNo() const{ return no; }
=======
    SSAGraphNode(int val) : val(val), type(CONST) {}
>>>>>>> 9cd9ef7951de5bfe3e8cd614e8d60b2c80fe9fdf
    void addChild(SSAGraphNode* node) { children.push_back(node); }
    std::vector<SSAGraphNode*>& getChildren() { return children; }
};

#endif