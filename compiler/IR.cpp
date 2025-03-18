#include "IR.h"

BasicBlock::BasicBlock(CFG* cfg, string entry_label){
    label = entry_label;
    cfg = cfg;
}

void BasicBlock::gen_asm(ostream &o){
    for_each(instrs.begin(), instrs.end(), gen_asm(o));
} /**< x86 assembly code generation for this basic block (very simple) */

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params){
    IRInstr ins = IRInstr(this, op, t, params);
    instrs.push_back(ins);
}
