#include "IR.h"

BasicBlock::BasicBlock(CFG* cfg, string entry_label){
    label = entry_label;
    cfg = cfg;
}

void BasicBlock::gen_asm(ostream &o){
    std::cout<< "@" << label <<":\n";
    for_each(instrs.begin(), instrs.end(), gen_asm(o));
    if(exit_true  && exit_false){
        std::cout<< "   cmp  $0, " << test_var_name << " \n";
        std::cout<< "   je " << exit_false->label << " \n";
        std::cout<< "   jmp @" << exit_true->label << " \n";
    }
    else if(exit_true && !exit_false){
        std::cout<< "   jmp @" << exit_true->label << " \n";
    }
    else{

    }
} /**< x86 assembly code generation for this basic block (very simple) */

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params){
    IRInstr ins = IRInstr(this, op, t, params);
    instrs.push_back(ins);
}
