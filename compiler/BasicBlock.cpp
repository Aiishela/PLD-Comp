#include "CFG.h"
#include "BasicBlock.h"
#include "IRInstr.h"

BasicBlock::BasicBlock(CFG* c, string entry_label){
    label = entry_label;
    cfg = c;
    exit_true = nullptr;
    exit_false = nullptr;
}

IRInstr *  BasicBlock::getLastInstr() {
    return instrs.back();
}

void BasicBlock::removeInstrs(int indexBegin, int indexEnd) {
    if (indexEnd == -1) {
        instrs.erase(next(instrs.begin(), indexBegin), instrs.end());
    } else {
        instrs.erase(next(instrs.begin(), indexBegin), next(instrs.begin(), indexEnd));
    }
}

void BasicBlock::gen_asm(ostream &o){
    o << "." << label << ":\n";
    
    for (auto it = instrs.begin(); it != instrs.end(); ++it) {
        (*it)->gen_asm(o);
    }
    if(exit_true  && exit_false){
        o << "   cmpl  $0, " << cfg->get_var_index(test_var_name) << "(%rbp)\n";
        o << "   je ." << exit_false->label << "        # exit_false\n";
        o << "   jmp ." << exit_true->label << "        # exit_true\n";
    }
    else if(exit_true && !exit_false){
        o << "   jmp ." << exit_true->label << "        # only exit_true \n";
    }
    else{

    }
} /**< x86 assembly code generation for this basic block (very simple) */

void BasicBlock::add_IRInstr(Operation op, Type t, vector<string> params){
    IRInstr * ins = new IRInstr(this, op, t, params);
    instrs.push_back(ins);
}

void BasicBlock::store_load_optim(){
    for (auto it = instrs.begin(); it != instrs.end(); ++it) {
        if ((*it)->getOp() == Operation::copy) {
            // Optimize movl %eax, -4(%rbp) (removes the second one)
            //          movl -4(%rbp), %eax
            auto next_it = std::next(it);
            if (next_it != instrs.end() && (*next_it)->getOp() == Operation::copy) {
                vector<string> params1 = (*it)->getParams();
                vector<string> params2 = (*next_it)->getParams();
                
                // Check if the first instruction stores %eax into a variable and the next loads it back
                if (params1[0] == params2[1] && params1[1] == params2[0]) {
                    instrs.erase(next_it);  // Remove the redundant load
                }
            }
        }
        else if ((*it)->getOp() == ldconst) {
            // Optimize movl $0, %eax   ( multiple ones in a row)
            //          movl $0, %eax
            auto next_it = std::next(it);
            if (next_it != instrs.end() && (*next_it)->getOp() == ldconst) {
                vector<string> params1 = (*it)->getParams();
                vector<string> params2 = (*next_it)->getParams();
                
                // Check if the they are the same instruction
                if (params1[1] == params2[1]) {
                    instrs.erase(next_it);
                    --it;
                }
            }
        }
    }
    
}