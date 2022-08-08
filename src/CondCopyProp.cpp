#include "CondCopyProp.h"
#include <vector>
#include <unordered_map>
#include <set>
#include <queue>
using namespace std;
const int N = 100;
const int top = 1e6, bottom = -1e6;
typedef vector<BasicBlock*>::iterator bb_iterator;
bool ExecFlag[N][N];
unordered_map<SymbolEntry*, int> LatCell;
unordered_map<int, vector<int>> SSASucc;
set<int> E;
map<int, Instruction*> Inst;
struct wl{
    int e1;
    int e2;
}WL, SSA[N];

queue<struct wl> FlowWL, SSAWL;
using namespace std;

bool EL(int k, int i){
    for(auto s:SSASucc[k]){
        if(i==s){
            return true;
        }
    }
    return false;
}

int Lat_Eval(Instruction* inst){
    int res = top;
    vector<Operand*> operands(inst->getOperands());
    // SymbolEntry* sym1 = operands[0]->getEntry();
    if(inst->isLoad()){
        SymbolEntry* sym2 = operands[1]->getEntry();
        if(sym2->isConstant())
            res = ((ConstantSymbolEntry*)sym2)->getValue();
        else if(sym2->isVariable())
            res = ((IdentifierSymbolEntry*)sym2)->getValue();
    }
    else if(inst->isBin()){
        enum { SUB, ADD, AND, OR, MUL, DIV, MOD };
        SymbolEntry *sym2 = operands[1]->getEntry(), *sym3 = operands[2]->getEntry();
        int v1, v2;
        if(sym2->isConstant())
            v1 = ((ConstantSymbolEntry*)sym2)->getValue();
        else if(sym2->isVariable())
            v1 = ((IdentifierSymbolEntry*)sym2)->getValue();
        if(sym3->isConstant())
            v2 = ((ConstantSymbolEntry*)sym3)->getValue();
        else if(sym3->isVariable())
            v2 = ((IdentifierSymbolEntry*)sym3)->getValue();
        switch(((BinaryInstruction*)inst)->getOp()){
            case SUB:
                res = v1-v2;
                break;
            case ADD:
                res = v1+v2;
                break;
            case AND:
                res = v1&&v2;
                break;
            case OR:
                res = v1||v2;
                break;
            case MUL:
                res = v1*v2;
                break;
            case DIV:
                res = v1/v2;
                break;
            case MOD:
                res = v1%v2;
                break;
        }
    }
    else if(inst->isCmp()){
        enum { E, NE, L, LE, G, GE };
        SymbolEntry *sym2 = operands[1]->getEntry(), *sym3 = operands[2]->getEntry();
        int v1, v2;
        if(sym2->isConstant())
            v1 = ((ConstantSymbolEntry*)sym2)->getValue();
        else if(sym2->isVariable())
            v1 = ((IdentifierSymbolEntry*)sym2)->getValue();
        if(sym3->isConstant())
            v2 = ((ConstantSymbolEntry*)sym3)->getValue();
        else if(sym3->isVariable())
            v2 = ((IdentifierSymbolEntry*)sym3)->getValue();
        switch(((CmpInstruction*)inst)->getOp()){
            case E:
                res = v1==v2;
                break;
            case NE:
                res = v1!=v2;
                break;
            case L:
                res = v1<v2;
                break;
            case LE:
                res = v1<=v2;
                break;
            case G:
                res = v1>v2;
                break;
            case GE:
                res = v1>=v2;
                break;
        }
    }
    return res;
}

int Edge_Count(int b){
    int res = 0;
    for(auto e:E){
        if(ExecFlag[e][b]){
            res++;
        }
    }
    return res;
}

void Visit_Phi(Instruction* inst){
    vector<Operand*> operands(inst->getOperands());
    SymbolEntry* sym1 = operands[0]->getEntry();
    map<BasicBlock*, Operand*> srcs(((PhiInstruction*)inst)->getSrcs());
    for(auto it : srcs){
        SymbolEntry* sym = it.second->getEntry();
        if(sym->isVariable() && ((IdentifierSymbolEntry*)sym)->getValue())
            LatCell[sym1] = ((IdentifierSymbolEntry*)sym)->getValue();
    }

}

void Visit_Inst(int b, Instruction* inst){
    // calculate
    vector<Operand*> operands(inst->getOperands());
    int val = Lat_Eval(inst);
    if(operands[0] && val != LatCell[operands[0]->getEntry()]){
        LatCell[operands[0]->getEntry()] = val;
        for(auto s: SSASucc[b]){
            SSAWL.push({b, s});
        }
    }
    if(inst->isBin()){
        if(val==top){
            for(auto i: SSASucc[b]){
                FlowWL.push({b, i});
            }
        }
        else if(val != bottom){
            if(SSASucc[b].size()==2){
                for(auto i: SSASucc[b]){
                    if((val && EL(b, i))||(!val && !EL(b, i))){
                        FlowWL.push({b, i});
                    }

                }
            }
            else if(SSASucc[b].size()==1){
                for(auto s:SSASucc[b]){
                    FlowWL.push({b, s});
                }
            }
        }
    }
}



void Initialize(Unit* unit){
    auto iter = unit->begin();
    while (iter != unit->end()){
        MinSSAGraph* ssa = (*iter)->getMinSSA();
        SSAGraphNode* node = ssa->getEntry();
        FlowWL.push({0, node->getNo()});
        for(auto n1: E){
            for(auto n2: E){
                ExecFlag[n1][n2] = false;
            }
        }
        // vector<SSAGraphNode*> nodes((*iter)->getNodes());
        // for(auto node : nodes){
        //     Instruction* inst = node->getInst();
        //     if(inst){
        //         vector<Operand*> operands(inst->getOperands());
        //         if(operands[0])
        //             LatCell[operands[0]->getEntry()] = top;
        //     }
        // }
        iter++;
    }
}

void CondCopyProp::pass(){
    // create the minimize SSA: unit
    int num = 1, idx = 0;
    E.insert(0); // as Entry
    auto iter = unit->begin();
    while (iter != unit->end()){
        (*iter)->genSSAGraph();
        vector<SSAGraphNode*> nodes((*iter)->getNodes());
        for(auto i : nodes){
            ((SSAGraphNode*)i)->setNo(num);
            Inst[num] = i->getInst();
            E.insert(num++);
        }
        vector<BasicBlock*> pred, succ;
        bool st[100];
        BasicBlock* bb = (*iter)->getEntry();
        queue<BasicBlock*> q;
        q.push(bb);
        st[bb->getNo()] = true;
        SSAGraphNode* first = nullptr;
        while(!q.empty()){
            BasicBlock* bb = q.front();
            q.pop();
            SSAGraphNode* tmp = nullptr;
            for(auto inst = bb->begin(); inst != bb->end(); inst = inst->getNext()){
                SSAGraphNode* node = inst->getNode();
                if(node){
                    if(!first){
                        first = node;
                    }
                    if(tmp) {
                        SSA[idx++] = {tmp->getNo(), node->getNo()};
                        // cout<<tmp->getNo()<<"->"<<node->getNo()<<endl;
                        SSASucc[tmp->getNo()].push_back(node->getNo());
                    }
                    tmp = node;
                }                
            }
            for(auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++){
                bool add = false;
                for(auto inst = (*succ)->begin(); inst != (*succ)->end(); inst=inst->getNext()){
                    SSAGraphNode* node = inst->getNode();
                    if(node){
                        if(tmp) {
                            // tmp->addSucc(node);
                            // node->addPred(tmp);
                            SSA[idx++] = {tmp->getNo(), node->getNo()};
                            // cout<<tmp->getNo()<<"->"<<node->getNo()<<endl;
                            SSASucc[tmp->getNo()].push_back(node->getNo());
                        }
                        add = true;
                        break;
                    }
                }
                if(add && !st[(*succ)->getNo()]){
                    q.push(*succ);
                    st[(*succ)->getNo()] = true;
                }
                else if(!add) {
                    for(auto succ1 = (*succ)->succ_begin(); succ1 != (*succ)->succ_end(); succ1++){
                        bool add = false;
                        for(auto inst = (*succ1)->begin(); inst != (*succ1)->end(); inst=inst->getNext()){
                            SSAGraphNode* node = inst->getNode();
                            if(node){
                                if(tmp) {
                                    SSA[idx++] = {tmp->getNo(), node->getNo()};
                                    // cout<<tmp->getNo()<<"->"<<node->getNo()<<endl;
                                    SSASucc[tmp->getNo()].push_back(node->getNo());
                                }
                                add = true;
                                break;
                            }
                        }
                        if(add && !st[(*succ1)->getNo()]){
                            q.push(*succ1);
                        }
                    }
                }
            }
        }
        (*iter)->setMinSSA(first);
        iter++;
    }   
    Initialize(unit);
    while(FlowWL.size() || SSAWL.size()){
        if(FlowWL.size()){
            WL = FlowWL.front();
            int a = WL.e1, b = WL.e2;
            FlowWL.pop();
            if(!ExecFlag[a][b]){
                ExecFlag[a][b] = true;
                if(Inst[b]->isPhi()){
                    Visit_Phi(Inst[b]);
                } else if(Edge_Count(b) == 1){
                    Visit_Inst(b, Inst[b]);
                }
            }
        }
        if(SSAWL.size()){
            WL = SSAWL.front();
            // int a = WL.e1;
            int b = WL.e2;
            SSAWL.pop();
            if(Inst[b]->isPhi()){
                Visit_Phi(Inst[b]);
            } else if(Edge_Count(b) >= 1){
                Visit_Inst(b, Inst[b]);
            }
        }
    }
    // use the const result to replace the vars
    // and eliminate the dead codes
    iter = unit->begin();
    queue<BasicBlock*> q;
    bool st[100];
    while(iter != unit->end()){
        BasicBlock* bb = (*iter)->getEntry();
        q.push(bb);
        st[bb->getNo()] = true;
        while(!q.empty()){
            bb = q.front();
            q.pop();
            Instruction* inst = bb->begin();
            vector<Operand*> operands(inst->getOperands());            
            if(inst->isLoad() || inst->isBin()){
                if(LatCell.find(operands[0]->getEntry()) != LatCell.end()){
                    Instruction* newInst = new LoadInstruction(operands[0],
                    new Operand(new ConstantSymbolEntry(TypeSystem::intType, LatCell[operands[0]->getEntry()])), nullptr);
                    bb->insertBefore(newInst, inst);                    
                    bb->remove(inst);
                }
            }
            if(inst->isCmp()){
                // bool branch = false, to = false;
                if(LatCell.find(operands[0]->getEntry()) != LatCell.end()){
                    Instruction* newInst = new LoadInstruction(operands[0],
                    new Operand(new ConstantSymbolEntry(TypeSystem::intType, LatCell[operands[0]->getEntry()])), nullptr);
                    bb->insertBefore(newInst, inst);                    
                    bb->remove(inst);
                }
                else if(LatCell.find(operands[1]->getEntry())!= LatCell.end() && LatCell.find(operands[2]->getEntry()) != LatCell.end()){
                    int v1 = LatCell[operands[1]->getEntry()], v2 = LatCell[operands[2]->getEntry()];
                    bool v = false;
                    enum { E, NE, L, LE, G, GE };
                    switch(((CmpInstruction*)inst)->getOp()) {
                        case E:
                            if(v1 == v2) v = true;
                            break;
                        case NE:
                            if(v1 != v2) v = true;
                            break;
                        case L:
                            if(v1 < v2) v = true;
                            break;
                        case LE:
                            if(v1 <= v2) v = true;
                            break;
                        case G:
                            if(v1 > v2) v = true;
                            break;
                        case GE:
                            if(v1 >= v2) v = true;
                            break;
                    }
                    Instruction* newInst = new LoadInstruction(operands[0],
                    new Operand(new ConstantSymbolEntry(TypeSystem::boolType, v)), nullptr);
                    bb->insertBefore(newInst, inst);                    
                    bb->remove(inst);
                }
                else if(LatCell.find(operands[1]->getEntry())!= LatCell.end() && LatCell.find(operands[2]->getEntry()) == LatCell.end()){
                    Instruction* newInst = new CmpInstruction(((CmpInstruction*)inst)->getOp(), operands[0],
                    new Operand(new ConstantSymbolEntry(TypeSystem::intType, LatCell[operands[1]->getEntry()])), 
                    operands[2],nullptr);
                    bb->insertBefore(newInst, inst);                    
                    bb->remove(inst);
                }
                else if(LatCell.find(operands[1]->getEntry())== LatCell.end() && LatCell.find(operands[2]->getEntry()) != LatCell.end()){
                    Instruction* newInst = new CmpInstruction(((CmpInstruction*)inst)->getOp(), operands[0],
                    operands[1],
                    new Operand(new ConstantSymbolEntry(TypeSystem::intType, LatCell[operands[2]->getEntry()])), 
                    nullptr);
                    bb->insertBefore(newInst, inst);                    
                    bb->remove(inst);
                }
            }
            for(auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++){
                if(!st[(*succ)->getNo()]){
                    q.push(*succ);
                }
            }
        }
        iter++;
    }
}

CondCopyProp::~CondCopyProp(){
    LatCell.clear(), SSASucc.clear(), E.clear(), Inst.clear();
}