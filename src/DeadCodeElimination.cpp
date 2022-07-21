#include "DeadCodeElimination.h"
#include <vector>

using namespace std;

void DeadCodeElimination::mark(Function* func) {
    // vector<Instruction*> worklist;
    // for (auto it = func->begin(); it != func->end(); it++) {
    //     (*it)->cleanMark();
    //     for (auto it1 = (*it)->begin(); it1 != (*it)->end(); it1++) {
    //         if (it1->isEssential()) {
    //             it1->setMark();
    //             worklist.push_back(it1);
    //         }
    //     }
    // }

    // while (!worklist.empty()) {
    //     auto ins = worklist.back();
    //     worklist.pop_back();
    //     auto uses = ins->getUse();
    //     for (auto it : uses){
    //         it->getDef()->setMark();
    //     }
    // }
}