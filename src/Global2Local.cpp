#include "Global2Local.h"
#include <numeric>

using namespace std;

void Global2Local::pass() {
    calGlobals();
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void Global2Local::pass(Function* func) {
    auto globals = usedGlobals[func];
    if (globals.empty())
        return;
    map<SymbolEntry*, Operand*> g2l;
    for (auto g : globals) {
        auto type = ((PointerType*)(g->getType()))->getType();
        if (type->isArray())
            continue;
        auto ins = this->globals[g][func];
        auto newSe = new TemporarySymbolEntry(new PointerType(type),
                                              SymbolTable::getLabel());
        auto newAddr = new Operand(newSe);
        g2l[g] = newAddr;
        auto entry = func->getEntry();
        // 函数入口load 出口store 调其他函数 如果用了store过的global 前面需store 若函数store global 返回需load
        // 这里需要随便来一个se type对就行
        auto alloc =
            new AllocaInstruction(newAddr, new TemporarySymbolEntry(type, 0));
        alloc->setParent(entry);
        entry->insertFront(alloc);
        SymbolEntry* addr_se;
        addr_se = new IdentifierSymbolEntry(*((IdentifierSymbolEntry*)g));
        addr_se->setType(new PointerType(type));
        auto addr = new Operand(addr_se);
        auto dst = new Operand(
            new TemporarySymbolEntry(type, SymbolTable::getLabel()));
        auto load = new LoadInstruction(dst, addr);
        load->setParent(entry);
        for (auto in = entry->begin(); in != entry->end(); in = in->getNext())
            if (!in->isAlloc()) {
                entry->insertBefore(load, in);
                break;
            }
        auto store = new StoreInstruction(newAddr, dst);
        store->setParent(entry);
        entry->insertAfter(store, load);
        for (auto in : ins) {
            if (in->isLoad()) {
                auto addr = in->getUse()[0];
                in->replaceUse(addr, newAddr);
            } else if (in->isStore()) {
                auto addr = in->getUse()[0];
                in->replaceUse(addr, newAddr);
            } else
                assert(0);
        }
    }
    for (auto block : func->getBlockList()) {
        auto in = block->rbegin();
        if (in->isRet())
            for (auto it : write[func]) {
                if (!g2l[it])
                    continue;
                auto type = ((PointerType*)(it->getType()))->getType();
                if (type->isArray())
                    continue;
                auto addr_se =
                    new IdentifierSymbolEntry(*((IdentifierSymbolEntry*)it));
                addr_se->setType(new PointerType(type));
                auto addr = new Operand(addr_se);
                auto dst = new Operand(
                    new TemporarySymbolEntry(type, SymbolTable::getLabel()));
                auto load = new LoadInstruction(dst, g2l[it]);
                load->setParent(block);
                block->insertBefore(load, in);
                auto store = new StoreInstruction(addr, dst);
                store->setParent(block);
                block->insertBefore(store, in);
            }
    }
    for (auto block : func->getBlockList()) {
        for (auto in = block->begin(); in != block->end(); in = in->getNext()) {
            if (in->isCall()) {
                auto f = ((IdentifierSymbolEntry*)(((CallInstruction*)in)
                                                       ->getFuncSE()))
                             ->getFunction();
                if (!f)
                    continue;
                for (auto g : read[f]) {
                    auto type = ((PointerType*)(g->getType()))->getType();
                    if (type->isArray())
                        continue;
                    // 此时函数没有用到g 也就没必要多这一步了
                    if (!g2l[g])
                        continue;
                    auto addr_se =
                        new IdentifierSymbolEntry(*((IdentifierSymbolEntry*)g));
                    addr_se->setType(new PointerType(type));
                    auto addr = new Operand(addr_se);
                    auto dst = new Operand(new TemporarySymbolEntry(
                        type, SymbolTable::getLabel()));
                    auto load = new LoadInstruction(dst, g2l[g]);
                    load->setParent(block);
                    block->insertBefore(load, in);
                    auto store = new StoreInstruction(addr, dst);
                    store->setParent(block);
                    block->insertBefore(store, in);
                }
                for (auto g : write[f]) {
                    auto type = ((PointerType*)(g->getType()))->getType();
                    if (type->isArray())
                        continue;
                    if (!g2l[g])
                        continue;
                    auto addr_se =
                        new IdentifierSymbolEntry(*((IdentifierSymbolEntry*)g));
                    addr_se->setType(new PointerType(type));
                    auto addr = new Operand(addr_se);
                    auto dst = new Operand(new TemporarySymbolEntry(
                        type, SymbolTable::getLabel()));
                    auto load = new LoadInstruction(dst, addr);
                    load->setParent(block);
                    auto store = new StoreInstruction(g2l[g], dst);
                    store->setParent(block);
                    block->insertAfter(store, in);
                    block->insertAfter(load, in);
                }
            }
        }
    }
}

void Global2Local::calGlobals() {
    for (auto g : unit->getGlobals())
        globals.insert({g, {}});
    map<Function*, int> func2idx;
    int idx = 0;
    for (auto it = unit->begin(); it != unit->end(); it++) {
        func2idx[*it] = idx++;
        for (auto block : (*it)->getBlockList())
            for (auto in = block->begin(); in != block->end();
                 in = in->getNext())
                for (auto u : in->getUse())
                    if (u->isGlobal()) {
                        auto entry = u->getEntry();
                        if (((IdentifierSymbolEntry*)entry)->getConst())
                            continue;
                        globals[entry][*it].push_back(in);
                        usedGlobals[*it].insert(entry);
                        if (in->isLoad())
                            read[*it].insert(entry);
                        else if (in->isStore())
                            write[*it].insert(entry);
                    }
    }
    vector<vector<int>> matrix(idx, vector<int>(idx));
    for (auto it : func2idx)
        for (auto it1 : it.first->getPreds())
            matrix[func2idx[it1.first]][it.second] = 1;
    vector<int> outDeg(idx, 0);
    for (int i = 0; i < idx; i++) {
        matrix[i][i] = 0;
        outDeg[i] = accumulate(matrix[i].begin(), matrix[i].end(), 0);
    }
    int finish = 0;
    while (finish < idx) {
        int i;
        for (i = 0; i < idx; i++)
            if (outDeg[i] == 0)
                break;
        outDeg[i] = -1;
        finish++;
        auto func = *(unit->begin() + i);
        for (auto it : func->getPreds()) {
            read[it.first].insert(read[func].begin(), read[func].end());
            write[it.first].insert(write[func].begin(), write[func].end());
            outDeg[func2idx[it.first]]--;
        }
    }
    write[unit->getMain()].clear();
}