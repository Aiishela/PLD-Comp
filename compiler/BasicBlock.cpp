#include "CFG.h"
#include "BasicBlock.h"
#include "IRInstr.h"

BasicBlock::BasicBlock(CFG* c, string entry_label){
    label = entry_label;
    cfg = c;
    exit_true = nullptr;
    exit_false = nullptr;
}

void BasicBlock::gen_asm(ostream &o){
    o << "." << label << ":\n";

    for (auto it = instrs.begin(); it != instrs.end(); ++it) {
        (*it)->gen_asm(o);
    }
    if(exit_true  && exit_false){
        o << "   cmp  $0, " << test_var_name << " \n";
        o << "   je ." << exit_false->label << " \n";
        o << "   jmp ." << exit_true->label << " \n";
    }
    else if(exit_true && !exit_false){
        o << "   jmp ." << exit_true->label << " \n";
    }
    else{

    }
} /**< x86 assembly code generation for this basic block (very simple) */

void BasicBlock::add_IRInstr(Operation op, Type t, vector<string> params){
    IRInstr * ins = new IRInstr(this, op, t, params);
    instrs.push_back(ins);
}