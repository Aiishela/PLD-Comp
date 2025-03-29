#include "BasicBlock.h"
#include "IRInstr.h"
#include "CFG.h"

using namespace std;

IRInstr::IRInstr(BasicBlock* bb_, Operation op_, Type t_, vector<string> params_)
    : bb(bb_), op(op_), t(t_), params(params_) {}

void IRInstr::gen_asm(ostream &o) {
    switch (op) {
        case ldconst: //var=const
            o << "   movl $" << params[1] << ", %eax\n";    
            break;
        case Operation::copy: //var0=var1
            if (params[1] == "!reg") { // %eax dans var0
                o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) <<"(%rbp)\n";

            } else if (params[0] == "!reg") { // var1 dans %eax
                o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n"; 

            } else { // var1 dans var0
                o << "   movl " << bb->cfg->get_var_index(params[1]) <<"(%rbp), %eax\n"; 
                o << "   movl %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)\n"; 

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
        case div_: //var0=var0/var1

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

