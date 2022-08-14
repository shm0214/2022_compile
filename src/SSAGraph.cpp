#include "SSAGraph.h"

BasicBlock* SSAGraphNode::getDefBB(){
    if(this->type!=CONST){
        return this->ins->getParent();
    }
    else{
        // assert("visit CONST node INSTRCTION!");
        return nullptr;
    }
}

Operand* SSAGraphNode::getOperator(){
    if(this->type!=CONST){
        return this->ins->getDef();
    }
    else{
        // assert("visit CONST node INSTRCTION!");
        return new Operand(new ConstantSymbolEntry(TypeSystem::intType,val));
    }
}

SSAGraphNode* SSAGraphNode::IV_clone(Operand* result){

    if(this->ins==nullptr){
        assert("instruction not exist");
    }

    Instruction* copy=this->ins->copy();
    copy->replaceDef(result);
    BasicBlock* fartherBB = this->ins->getParent();

    fartherBB->insertBefore(copy,this->ins->getNext());

    SSAGraphNode* node=new SSAGraphNode(copy,this->type);

    node->setHeader(this->header);
    node->setlow(this->low);
    node->setnum(this->num);
    node->setvist();

    for(auto child:this->children){
        node->addChild(child);
    }

    return node;
}

void SSAGraphNode::removeChild(SSAGraphNode* node){
    for(std::vector<SSAGraphNode*>::iterator iter=children.begin();iter!=children.end();iter++){        //从vector中删除指定的某一个元素 
        if(*iter==node){
            children.erase(iter);
            iter--;
        }
    }
}