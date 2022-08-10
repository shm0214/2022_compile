#ifndef __SSA_GRAPH_H__
#define __SSA_GRAPH_H__
#include <vector>

class Instruction;
class SSAGraphNode;

class MinSSAGraph {
    SSAGraphNode* head;
  public:
    MinSSAGraph(SSAGraphNode* node) { 
        head = node;
    }
    SSAGraphNode* getEntry() const{ return head; }
};

class SSAGraphNode {
    // CONST 没有
    Instruction* ins;
    // only for CONST
    int val;
    int type;
    int no;
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
        // STORE,
        GEP,
        ZEXT,
        XOR,
        BITCAST,
        SHL,
        ASHR,
    };
    SSAGraphNode(Instruction* ins, int type = -1) : ins(ins), type(type) {}
    SSAGraphNode(Instruction* ins, int val, int type) : ins(ins), val(val), type(CONST) {}
    SSAGraphNode() : no(0) {} // only for entry node
    Instruction* getInst() { return ins; }
    void setNo(int num) { no = num; }
    int getNo() const{ return no; }
    void addChild(SSAGraphNode* node) { children.push_back(node); }
    std::vector<SSAGraphNode*>& getChildren() { return children; }     
};

#endif