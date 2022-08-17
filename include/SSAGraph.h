#ifndef __SSA_GRAPH_H__
#define __SSA_GRAPH_H__
#include <assert.h>
#include <vector>
#include "Instruction.h"
#include "BasicBlock.h"
#include "Operand.h"

class SSAGraphNode {
    // CONST 没有
    Instruction* ins;
    // only for CONST
    int val;
    int type;
    bool visited;
    int num;
    int low;

    Operand* globalOperand;
    SSAGraphNode* header;
    std::vector<SSAGraphNode*> children;
    SSAGraphNode* origin;

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
        GLOBAL,
        FUNCPARA,
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
    SSAGraphNode(){visited=false;};
    SSAGraphNode(Instruction* ins, int type = -1) : ins(ins), type(type), visited(false) {header=new SSAGraphNode();origin=header;}
    SSAGraphNode(int val) : val(val), type(CONST),visited(false) {header=new SSAGraphNode();origin=header;}
    SSAGraphNode(Operand* op,int type):visited(false){this->type=type; this->globalOperand=op;};
    void addChild(SSAGraphNode* node) { children.push_back(node); }
    void removeChild(SSAGraphNode* node);
    std::vector<SSAGraphNode*>& getChildren() { return children; }
    bool isvisit(){return visited==true;};
    void setvist(){visited=true;};
    void resetvisit(){visited=false;};
    void setnum(int num){this->num=num;};
    int getnum(){return num;};
    void setlow(int low){this->low=low;};
    int getlow(){return low;};
    int getType(){return type;};
    int getvalue(){return val;};
    SSAGraphNode* getHeader(){return header;};
    void setHeader(SSAGraphNode* header){this->header=header;};
    SSAGraphNode* getorigin(){return this->origin;};
    void resetHeader(){this->header=origin;};
    BasicBlock* getDefBB();
    Operand* getOperator();
    Instruction* getIns(){return ins;};
    void setIns(Instruction* ins){this->ins=ins;};
    SSAGraphNode* IV_clone(Operand* result);
};


#endif