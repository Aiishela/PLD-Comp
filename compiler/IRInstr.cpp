#include "BasicBlock.h"
#include "IRInstr.h"
#include <iomanip>
#include "CFG.h"

#define PRINT_VAR_INFO(N,s) setw(10) <<"# " << s << " " << params[N] << " (" << bb->cfg->get_var_index(params[N]) << ")"

using namespace std;

IRInstr::IRInstr(BasicBlock* bb_, Operation op_, Type t_, vector<string> params_)
    : bb(bb_), op(op_), t(t_), params(params_) {}
    
int IRInstr::getConstValue(){
    return stoi(params[1]);
}

void IRInstr::gen_asm(ostream &o) {
    switch (op) {
        case ldconst: //var=const
            o << "   movl $" << params[1] << ", %eax\n";    
            break;
        case swap_:  
            o << "   movl %eax, %ecx\n"; 
            o << "   movl " << bb->cfg->get_var_index(params[0]) << "(%rbp), %eax" << PRINT_VAR_INFO(0,"swap_") << "\n";
            o << "   movl %ecx, " << bb->cfg->get_var_index(params[0]) << "(%rbp)\n";
            break;
        case jmp:
            o << "   jmp ." << params[0] << "\n"; 
            break;
        case Operation::copy: //var0=var1
            if (params[1] == "!reg") { // %eax dans var0
                o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)" << PRINT_VAR_INFO(0,"copy") << "\n";

            } else if (params[0] == "!reg") { // var1 dans %eax
                o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"copy") << "\n";

            } else { // var1 dans var0
                o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"copy") << "\n";
                o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << PRINT_VAR_INFO(0,"copy") << "\n";

            }
            break;
        case add: //var0=var0+var1
            o << "   addl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"add") << "\n";
            break;
        case sub: //var0=var0-var1
            o << "   subl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"sub") << "\n";
            break;
        case postIncr: //var0=var0+1  DO NOT CHANGE THE FLAGS
            o << "   leal	1(%rax), %edx\n" ; 
            o << "   movl	%edx, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << PRINT_VAR_INFO(0,"postIncr") << "\n";
            break;
        case postDecr: //var0=var0-1
            o << "   leal	-1(%rax), %edx\n" ; 
            o << "   movl	%edx, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << PRINT_VAR_INFO(0,"postDecr") << "\n";
            break;
        case preIncr: //var0=var0+1 CHANGE THE FLAGS
            o << "   addl	$1, "<< bb->cfg->get_var_index(params[0]) << "(%rbp)" << PRINT_VAR_INFO(0,"preIncr") << "\n";
            break;
        case preDecr: //var0=var0-1
            o << "   subl	$1, " << bb->cfg->get_var_index(params[0]) << "(%rbp)" << PRINT_VAR_INFO(0,"preDecr") << "\n";
            break;
        case mul: //var0=var0*var1
            //On fait la multiplication et on la met dans var0
            o << "   imull " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"mul") << "\n" ; 
            break;
        case div_: //var0=var0/var1

            //on passe en 64 bits
            o << "   cltd\n";

            //On fait la division et on la met dans var0
            o << "   idivl " << bb->cfg->get_var_index(params[1]) <<"(%rbp)" << PRINT_VAR_INFO(1,"div") << "\n" ; 
            break;
        case mod: //var0=var0%var1
            //on passe en 64 bits
            o << "   cltd\n";

            //On fait la division et on la met dans var0
            o << "   idivl " << bb->cfg->get_var_index(params[1]) <<"(%rbp)" << PRINT_VAR_INFO(1,"mod") << "\n" ; 
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
            o << "   andl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"andbb") << "\n" ; 
            break;
        case notbb: //var0=~var0
            o << "   notl %eax\n" ;
            break;
        case orbb: //var0=var0|var1
            o << "   orl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"orbb") << "\n" ; 
            break;
        case rmem: //dest=*addr
            o << "   movl " << params[1] <<"(%rbp), %eax\n";
            o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)" << PRINT_VAR_INFO(0,"rmem") << "\n" ; 
            break;
        case wmem: //*var0=var1
            o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"wmem") << "\n";
            o << "   movl %eax, (" << bb->cfg->get_var_index(params[0]) << "(%rbp))" << PRINT_VAR_INFO(0,"wmem") << "\n";
            break;
        case call: { //var0(var1, var2, ..., var6)
            int nb_params = params.size(); // params[0] is the function name, params[1..] are arguments
        
            // List of argument registers in System V x86-64 calling convention
            std::vector<std::string> registers = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
        
            // Push arguments into registers (starting from params[1] up to 6 max)
            for (int i = 1; i < nb_params && i - 1 < 6; ++i) {
                o << "   movl " << bb->cfg->get_var_index(params[i]) << "(%rbp), " << registers[i - 1] << "\n";
            }
        
            // Call the function whose name is in params[0]
            o << "   call " << params[0] << "\n";
            break;
        }
        case cmp_eq: //var0=(var0==var1)
            o << "   cmpl %eax, " << bb->cfg->get_var_index(params[1]) <<"(%rbp)" << PRINT_VAR_INFO(1,"cmp_eq") << "\n" ; // compare gauche < droite 
            o << "   sete %al\n" ; 
            o << "   movzbl	%al, %eax\n" ; 
           break;
        case cmp_lt: //var0=(var1<var2)
            o << "   cmpl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"cmp_lt") << "\n" ; // compare droite < gauche 
            o << "   setl %al\n" ; 
            o << "   movzbl	%al, %eax" << endl ;
            break;
        case cmp_gt: //var0=(var1>var2)
            o << "   cmpl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax" << PRINT_VAR_INFO(1,"cmp_gt") << "\n" ; // compare gauche < droite 
            o << "   setg %al\n" ; 
            o << "   movzbl	%al, %eax" << endl ;
            break;
        case cmp_neq: //var0=(var0!=var1)
            o << "   cmpl %eax, " << bb->cfg->get_var_index(params[1]) <<"(%rbp)" << PRINT_VAR_INFO(1,"cmp_neq") << "\n" ; // compare gauche < droite 
            o << "   setne %al\n" ; 
            o << "   movzbl	%al, %eax" << endl ; 
           
            break;
        case jmp_if_true:
            o << "   cmpl $0, " << bb->cfg->get_var_index(params[0]) << "(%rbp)\n"; // compare l'expression à 0
            o << "   jne ." << params[1] << endl; // jump si != 0 <=> expr1 vraie
            break;
        case jmp_if_false:
            o << "   cmpl $0, " << bb->cfg->get_var_index(params[0]) << "(%rbp)\n";; //test l'expression pour mettre le flag à zéro ou non
            o << "   je ." << params[1] << endl; // jump si == 0 <=> expr1 fausse
            break;
        case label:
            o << "." << params[0] << ":\n"; // écris juste un label (utile pour le OU et le ET logique)
            break;
        default:
            cerr << "Unknown operation" << endl;
            break;
    }
}

