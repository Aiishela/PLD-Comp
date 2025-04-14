#include "BasicBlock.h"
#include "SymbolTable.h"
#include "CFG.h"

//Génération de code assembleur.
CFG::CFG(string name){
    // should create 3 blocks, 1=prolog, 2=currentblock, 3=epilogue
    funcName = name;
    symbolTable = new SymbolTable;
    symbolTableStack = std::vector<SymbolTable*>();
    symbolTableStack.push_back(symbolTable);
    nextTempIndex = 0;
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
    o << ".epilogue:\n";
    o << "   popq %rbp\n" ;
    o << "   ret\n";
}

string CFG::IR_reg_to_asm(string reg){
    string res = "-" + reg + "(%rbp)\n";
    return res;
}

//Gestion de la table des symboles

void CFG::use_variable(string name, int line, int col){
    (*symbolTableStack.rbegin())->useVariable(name, line, col);
}

//Gestion de la pile de table des symboles
void CFG::push_symbol_table(){

    symbolTable = new SymbolTable();
    symbolTableStack.push_back(new SymbolTable());
    std::cerr << " symbol table pushed, current stack size: " << symbolTableStack.size() << std::endl;
}

void CFG::pop_symbol_table() {
    

    if (symbolTableStack.size() > 1) {  // ne pas pop la dernière (table globale de la fonction)
        SymbolTable* toDelete = symbolTableStack.back(); // stocke l'ancien
        symbolTableStack.pop_back(); // retire de la pile
        delete toDelete; // supprime l'objet
        symbolTable = symbolTableStack.back();
        std::cerr << " symbol table popped, current stack size: " << symbolTableStack.size() << std::endl;
    } else {
        cerr << "Error: No symbol table to pop (or trying to pop the global one)!" << endl;
    }
    
}

int CFG::get_var_index(string name) {
    // Parcours la pile de tables de symboles, de la plus récente (au sommet) à la plus ancienne (à la base)
    for (auto it = symbolTableStack.rbegin(); it != symbolTableStack.rend(); ++it) {
        SymbolTable* st = *it;  // Obtenez la table de symboles à cet endroit dans la pile

        if (st->existVariable(name)) {  // Si la variable est trouvée dans la table de symboles
            std::cerr << "Variable " << name << " found in symbol table at index " << st->getVariableInfo(name).index << endl;
            return st->getVariableInfo(name).index;  // Retourner l'index de la variable
        }
        
    }

    // Si la variable n'a pas été trouvée après avoir parcouru toute la pile, afficher une erreur
    cerr << "Error: Variable '" << name << "' is not found in symbol stack!" << endl;
    return -1;  
}
void CFG::add_variable_st(const string& name, Type t, int line, int col) {
   
    /*if ( !( (*symbolTableStack.rbegin())->existVariable(name) ) ) {  // Si la variable n'est pas trouvée dans la table de symboles
        (*symbolTableStack.rbegin())->addVariable(name, t);
        std::cerr << "Variable " << name << " added to symbol table." << endl;
     }else{
        symbolTable->printError("Variable " + name + " is declared twice.", line, col);
     }*/
    (*symbolTableStack.rbegin())->addVariable(name, t, line, col);  // Ajoute la variable à la table de symboles

    
}

void CFG::add_variable_st(const string& name, Type t) {
   
    (*symbolTableStack.rbegin())->addVariable(name, t);  // Ajoute la variable à la table de symboles

}

void CFG::use_variable_st(const string& name, int line, int col) {
    bool found = false;
    for (auto it = symbolTableStack.rbegin(); it != symbolTableStack.rend(); ++it) {
        SymbolTable* symtab = *it;  // Obtenez la table de symboles à cet endroit dans la pile
        bool present = symtab->existVariable(name);

        if (present) {
            symtab->st->at(name).used = true;
            found = true;
            break;
        }
    }
    if (!found){
        (*symbolTableStack.rbegin())->printError("Variable " + name + " is not declared.", line, col);
    }
}

void CFG::checkUsageST(){
    symbolTable->checkUsageST();
}

void CFG::store_load_optim(){
    for (auto bb : bbs){
        bb->store_load_optim();
    }
}





Type CFG::get_var_type(string name){
    return symbolTable->getVariableInfo(name).type;
}

string CFG::new_BB_name(){
    return "BB" + to_string(nextBBnumber++);
}

string CFG::create_new_tempvar(Type t){
    string name = "tmp" + to_string(nextTempIndex++);
    add_variable_st(name, t, 0, 0);
    return name;
}