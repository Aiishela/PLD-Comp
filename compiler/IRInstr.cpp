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

void IRInstr::gen_asm_ARM(ostream &o) {
    string dst_offset = bb->cfg->get_var_index(params[0]);
    string src_offset = params.size() > 1 ? bb->cfg->get_var_index(params[1]) : "";

    switch (op) {
        case ldconst: // var = const
            o << "   mov r0, #" << params[1] << "\n";
            break;

        case Operation::copy: // var0 = var1
            if (params[1] == "!reg") {
                o << "   str r0, [fp, #" << dst_offset << "]\n"; //str -> store register
            } else if (params[0] == "!reg") {
                o << "   ldr r0, [fp, #" << src_offset << "]\n"; // ldr -> load register
            } else {
                o << "   ldr r0, [fp, #" << src_offset << "]\n"; // [fp, src_offset] -> correspond à l'adresse de la variable
                o << "   str r0, [fp, #" << dst_offset << "]\n";
            }
            break;

        case add: // var0 = var0 + var1
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   add r0, r0, r1\n"; // 
            break;

        case sub: // var0 = var0 - var1
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   sub r0, r0, r1\n";
            break;

        case mul: // var0 = var0 * var1
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   mul r0, r0, r1\n";
            break;

        case div_: // var0 = var0 / var1
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   bl __aeabi_idiv\n"; // r0 = r0 / r1 
            break;

        case mod: // var0 = var0 % var1
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   bl __aeabi_idivmod\n"; // r1 = r0 % r1, résultat dans r1
            o << "   mov r0, r1\n";
            break;

        case notU: // var0 = !var0
            o << "   cmp r0, #0\n"; 
            o << "   moveq r0, #1\n"; 
            o << "   movne r0, #0\n";
            break;

        case negU: // var0 = -var0
            o << "   rsb r0, r0, #0\n";
            break;

        case andbb: // var0 = var0 & var1
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   and r0, r0, r1\n";
            break;

        case notbb: // var0 = ~var0
            o << "   mvn r0, r0\n";
            break;

        case orbb: // var0 = var0 | var1
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   orr r0, r0, r1\n";
            break;

        case rmem: // dest = *addr
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   ldr r0, [r1]\n";
            o << "   str r0, [fp, #" << dst_offset << "]\n";
            break;

        case wmem: // *var0 = var1
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   ldr r2, [fp, #" << dst_offset << "]\n";
            o << "   str r1, [r2]\n";
            break;

        case call:
            break;

        case cmp_eq: // var0 = (var0 == var1)
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   cmp r0, r1\n";
            o << "   moveq r0, #1\n";
            o << "   movne r0, #0\n";
            break;

        case cmp_neq: // var0 = (var0 != var1)
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   cmp r0, r1\n";
            o << "   movne r0, #1\n";
            o << "   moveq r0, #0\n";
            break;

        case cmp_lt: // var0 = (var0 < var1)
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   cmp r0, r1\n";
            o << "   movlt r0, #1\n";
            o << "   movge r0, #0\n";
            break;

        case cmp_gt: // var0 = (var0 > var1)
            o << "   ldr r1, [fp, #" << src_offset << "]\n";
            o << "   cmp r0, r1\n";
            o << "   movgt r0, #1\n";
            o << "   movle r0, #0\n";
            break;

        default:
            cerr << "Unknown ARM operation" << endl;
            break;
    }
}


