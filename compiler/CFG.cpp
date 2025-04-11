#include "BasicBlock.h"
#include "SymbolTable.h"
#include "CFG.h"

//Génération de code assembleur.
CFG::CFG(string name){
    // should create 3 blocks, 1=prolog, 2=currentblock, 3=epilogue
    funcName = name;
    symbolTable = new SymbolTable;
    nextTempIndex = 0;
    nextBBnumber = 0;
    std::string func_first = "first" + funcName;
    current_bb = new BasicBlock(this, func_first);
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
    o << ".epilogue"<<funcName<<":\n";
    o << "   popq %rbp\n" ;
    o << "   ret\n";
}

string CFG::IR_reg_to_asm(string reg){
    string res = "-" + reg + "(%rbp)\n";
    return res;
}

//Gestion de la table des symboles
void CFG::add_to_symbol_table(string name, Type t, int line, int col){
    symbolTable->addVariable(name, t, line, col);
}

void CFG::add_to_symbol_table(string name, Type t, int size){
    symbolTable->addVariable(name, t, size);
}

void CFG::use_variable(string name, int line, int col){
    symbolTable->useVariable(name, line, col);
}

void CFG::define_variable(string name,int line, int col){
    symbolTable->defineVariable(name, line, col);
}

void CFG::checkUsageST(){
    symbolTable->checkUsageST();
}

void CFG::store_load_optim(){
    for (auto bb : bbs){
        bb->store_load_optim();
    }
}

int CFG::get_var_index(string name){
    if (!symbolTable->existVariable(name)) {
        cerr << "Error: Variable '" << name << "' not found in symbol table!" << endl;
    }
    return symbolTable->getVariableInfo(name).index;
}

Type CFG::get_var_type(string name){
    return symbolTable->getVariableInfo(name).type;
}

string CFG::new_BB_name(){
    return "BB" + to_string(nextBBnumber++);
}

string CFG::create_new_tempvar(Type t){
    string name = "tmp" + to_string(nextTempIndex++);
    add_to_symbol_table(name, t, 0, 0);
    return name;
}