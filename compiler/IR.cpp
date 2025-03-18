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



//Génération de code assembleur.
CFG::CFG(DefFonction* ast){
    this->ast = ast;
    
    nextFreeSymbolIndex = 0;
    nextBBnumber = 0;
    current_bb = new BasicBlock(this, ast->name);
    add_bb(current_bb);
}
void CFG::add_bb(BasicBlock* bb){

    bbs.push_back(bb);
}

void CFG::gen_asm(ostream &o){
    for (auto bb : bbs){
        bb->gen_asm(o);
    }
}

void CFG::gen_asm_prologue(ostream& o){
    o << "   # prologue\n" ;
    o << "   pushq %rbp \n" ;
    o << "   movq %rsp, %rbp\n" ;
}

void CFG::gen_asm_epilogue(ostream& o){
    o << "   # epilogue\n" ;
    o << "   popq %rbp\n" ;
    o << "   ret\n";
}

string CFG::IR_reg_to_asm(string reg){
    string res = "-" + reg + "(%rbp)\n";
    return res;
}

//Gestion de la table des symboles
void CFG::add_to_symbol_table(string name, Type t){
    SymbolType[name] = t;
    SymbolIndex[name] = nextFreeSymbolIndex++;
}
int CFG::get_var_indestring name){
    return SymbolIndex[name];
}

Type CFG::get_var_type(string name){
    return SymbolType[name];
}

string CFG::new_BB_name(){
    return "BB" + to_string(nextBBnumber++);
}

string CFG::create_new_tempvar(Type t){
    string name = "tmp" + to_string(nextFreeSymbolIndex);
    add_to_symbol_table(name, t);
    return name;
}