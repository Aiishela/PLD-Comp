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
        instrs.erase(next(instrs.begin(), indexBegin-1), instrs.end());
    } else {
        instrs.erase(next(instrs.begin(), indexBegin-1), next(instrs.begin(), indexEnd));
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