#include "SSAGraph.h"
#include "Operand.h"

class IV_record{
    SSAGraphNode* iv;
    SSAGraphNode* rc;
    int op;
public:
    IV_record(SSAGraphNode* iv,SSAGraphNode* rc,int op){
        this->iv=iv;
        this->rc=rc;
        this->op=op;
    }
    Operand* getIVOp(){return iv->getOperator();};
    SSAGraphNode* getiv(){return iv;};
    SSAGraphNode* getrc(){return rc;};
    int getOp(){return op;};
};
