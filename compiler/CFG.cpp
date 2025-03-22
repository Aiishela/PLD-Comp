#include "BasicBlock.h"
#include "CFG.h"

//Génération de code assembleur.
CFG::CFG(string name){
    // should create 3 blocks, 1=prolog, 2=currentblock, 3=epilogue
    funcName = name;
    nextFreeSymbolIndex = -4;
    nextBBnumber = 0;
    current_bb = new BasicBlock(this, "first");
    add_bb(current_bb);
}


void CFG::add_bb(BasicBlock* bb){

    bbs.push_back(bb);
}

void CFG::gen_asm(ostream &o){
    o << "   .globl	" << funcName << "\n";
    o << "   .type " << funcName << " , @function\n";
    o << funcName << ":\n";
    gen_asm_prologue(o);

    for (auto bb : bbs){
        bb->gen_asm(o);
    }
    
    gen_asm_epilogue(o);
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
    SymbolIndex[name] = nextFreeSymbolIndex;

    nextFreeSymbolIndex = nextFreeSymbolIndex-4;
}

int CFG::get_var_index(string name){
    if (SymbolIndex.find(name) == SymbolIndex.end()) {
        cerr << "Error: Variable '" << name << "' not found in symbol table!" << endl;
    }
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