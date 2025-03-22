#include "IR.h"

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

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params){
    IRInstr * ins = new IRInstr(this, op, t, params);
    instrs.push_back(ins);
}



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
        std::cerr << "Error: Variable '" << name << "' not found in symbol table!" << std::endl;
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

#include <iostream>

using namespace std;

IRInstr::IRInstr(BasicBlock* bb_, Operation op_, Type t_, vector<string> params_)
    : bb(bb_), op(op_), t(t_), params(params_) {}

void IRInstr::gen_asm(ostream &o) {
    switch (op) {
        case ldconst: //var=const
            o << "   movl $" << params[1] << ", %eax\n";    
            break;
        case copy: //var0=var1
            if (params[1] == "!reg") { // %eax dans var0
                o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n";

            } else if (params[0] == "!reg") { // var1 dans %eax
                o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n"; 

            } else { // var1 dans var0
                o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n"; 
                o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) << "\n"; 

            }
            break;
        case add: //var0=var0+var1
            o << "   addl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n" ; 
            break;
        case sub: //var0=var0-var1
            o << "   subl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n" ; 
            break;
        case mul: //var0=var0*var1
            //On fait la multiplication et on la met dans var0
            o << "   imull " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n" ; 
            break;
        case div: //var0=var0/var1

            //on passe en 64 bits
            o << "   cltd\n";

            //On fait la division et on la met dans var0
            o << "   idivl " << bb->cfg->get_var_index(params[1]) <<"(%rbp)\n" ; 
            break;
        case mod: //var0=var0%var1
            //on passe en 64 bits
            o << "   cltd\n";

            //On fait la division et on la met dans var0
            o << "   idivl " << bb->cfg->get_var_index(params[1]) <<"(%rbp)\n" ; 
            o << "   movl %edx, %eax\n" ; 
            break;
        case notU: //var0=!var0
            o << "   cmpl $0, %eax\n";      // compare la var, const ou l'expression booléene à 0
            o << "   sete %al\n";
            o << "   movzbl %al, %eax\n";

            break;
        case negU: //var0=-var0
            o << "   negl %eax\n";
            break;
        case andbb: //var0=var0&var1
            o << "   andl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n" ; 
            break;
        case notbb: //var0=~var0
            o << "   notl %eax\n" ;
            break;
        case orbb: //var0=var0|var1
            o << "   orl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n" ; 
            break;
        case rmem: //dest=*addr
            o << "   movl " << params[1] <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case wmem: //*var0=var1
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
            o << "   movl %eax, (" << bb->cfg->get_var_index(params[0]) << "(%rbp))\n";
            break;
        case call:
            break;
        case cmp_eq: //var0=(var0==var1)
            o << "   cmp %eax, " << bb->cfg->get_var_index(params[1]) <<"(%rbp)\n" ; // compare gauche < droite 
            o << "   sete %al\n" ; 
            o << "   movzbl	%al, %eax\n" ; 
           break;
        case cmp_lt: //var0=(var1<var2)
            o << "   cmp %eax, " << bb->cfg->get_var_index(params[1]) <<"(%rbp)\n" ; // compare gauche < droite 
            o << "   setl %al\n" ; 
            o << "   movzbl	%al, %eax" << endl ;
            break;
        case cmp_gt: //var0=(var1>var2)
            o << "   cmp %eax, " << bb->cfg->get_var_index(params[1]) <<"(%rbp)\n" ; // compare gauche < droite 
            o << "   setg %al\n" ; 
            o << "   movzbl	%al, %eax" << endl ;
            break;
        case cmp_neq: //var0=(var0!=var1)
            o << "   cmp %eax, " << bb->cfg->get_var_index(params[1]) <<"(%rbp)\n" ; // compare gauche < droite 
            o << "   setne %al\n" ; 
            o << "   movzbl	%al, %eax" << endl ; 
           
            break;
        default:
            cerr << "Unknown operation" << endl;
            break;
    }
}

CFG* cfg = nullptr;
