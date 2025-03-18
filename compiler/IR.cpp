#include "IR.h"
<<<<<<< HEAD

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
=======
#include <iostream>

using namespace std;

IRInstr::IRInstr(BasicBlock* bb_, Operation op_, Type t_, vector<string> params_)
    : bb(bb_), op(op_), t(t_), params(params_) {}

void IRInstr::gen_asm(ostream &o) {
    //string reg1, reg2, reg3;
    switch (op) {
        case ldconst: //var=const
            o << "    movl $" << params[1] << ", " << bb->cfg->get_var_index(params[0]) << "(%rbp)\n";
            break;
        case copy: //var0=var1
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
            o << "   movl %eax, "<< bb->cfg->get_var_index(params[0]) <<"(%rbp)\n";
            break;
        case add: //var0=var1+var2
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n"; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
        
            //On ajoute tmp (var2) à %eax (var1) et on la met dans var0
            o << "   addl " << bb->cfg->get_var_index(tmp) <<"(%rbp), %eax\n" ; 
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case sub: //var0=var1-var2
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
        
            //On soustrait tmp (var2) à %eax (var1) et on la met dans var0
            o << "   subl " << bb->cfg->get_var_index(tmp) <<"(%rbp), %eax\n" ; 
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case mul: //var0=var1*var2
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
        
            //On fait la multiplication et on la met dans var0
            o << "   imull " << bb->cfg->get_var_index(tmp) <<"(%rbp), %eax\n" ; 
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case div: //var0=var1/var2
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
        
            //on passe en 64 bits
            o << "   cltd\n";

            //On fait la division et on la met dans var0
            o << "   idivl " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case mod: //var0=var1%var2
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
        
            //on passe en 64 bits
            o << "   cltd\n";

            //On fait la division et on la met dans var0
            o << "   idivl " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 
            o << "   movl %edx, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case notU: //var0=!var1
            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
            o << "   cmpl $0, %eax\n" ;
            o << "   sete %al\n";
            o << "   movzbl %al, %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 

            break;
        case negU: //var0=-var1
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
            o << "   negl %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case andbb: //var0=var1&var2
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";

            o << "   andl " << bb->cfg->get_var_index(tmp) <<"(%rbp), %eax\n" ;
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case notbb: //var0=~var1
            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";

            o << "   notl %eax\n" ;
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 
            break;
        case orbb: //var0=var1|var2
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";

            o << "   orl " << bb->cfg->get_var_index(tmp) <<"(%rbp), %eax\n" ;
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n" ; 

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
        case cmp_eq: //var0=(var1==var2)
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
            
            // on fait la comparaison
            o << "   cmp %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; // compare gauche < droite 

            o << "   sete %al\n" ; 
            
            o << "   movzbl	%al, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)" << endl ; 
           break;
        case cmp_lt: //var0=(var1<var2)
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
            
            // on fait la comparaison
            o << "   cmp %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; // compare gauche < droite 

            o << "   setl %al\n" ; 
            
            o << "   movzbl	%al, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)" << endl ;
            break;
        case cmp_gt: //var0=(var1>var2)
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
            
            // on fait la comparaison
            o << "   cmp %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; // compare gauche < droite 

            o << "   setg %al\n" ; 
            
            o << "   movzbl	%al, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)" << endl ;
            break;
        case cmp_neq: //var0=(var1!=var2)
            string tmp = bb->cfg->create_new_tempvar(int);

            //On met var2 dans la variable !tmp
            o << "   movl " << bb->cfg->get_var_index(params[2]) <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; 

            //On met var1 dans %eax
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n";
            
            // on fait la comparaison
            o << "   cmp %eax, " << bb->cfg->get_var_index(tmp) <<"(%rbp)\n" ; // compare gauche < droite 

            o << "   setne %al\n" ; 
            
            o << "   movzbl	%al, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)" << endl ; 
           
            break;
        default:
            cerr << "Unknown operation" << endl;
            break;
    }
}

CFG* cfg = nullptr;
>>>>>>> 0804ecd (mise en place de IRInstr)
