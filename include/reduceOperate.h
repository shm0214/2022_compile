#ifndef __REDUCE_OPERATE_H__
#define __REDUCE_OPERATE_H__

class Operand;

//_new = _old opcode op
class reduceOperate
{
private:
    int opcode;
    Operand* _old;
    Operand* _new;
    Operand* op;
public:
    reduceOperate(Operand* _new,
        Operand* _old,
        int opcode,
        Operand* op):_new(_new),_old(_old),opcode(opcode),op(op){};

    Operand* getold(){return _old;}
    Operand* getnew(){return _new;}
    Operand* getop(){return op;}
    int getOpCode(){return opcode;}
};


#endif