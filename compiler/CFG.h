#ifndef CFG_H
#define CFG_H

#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>
#include <map>

#include "Type.h"
#include "Operation.h"
#include "SymbolTable.h"

class BasicBlock;

using namespace std;

/** The class for the control flow graph, also includes the symbol table */

/* A few important comments:
	 The entry block is the one with the same label as the AST function name.
	   (it could be the first of bbs, or it could be defined by an attribute value)
	 The exit block is the one with both exit pointers equal to nullptr.
     (again it could be identified in a more explicit way)

 */
class CFG {
    public:
       CFG(string name);
   
       void add_bb(BasicBlock* bb); 
   
       // x86 code generation: could be encapsulated in a processor class in a retargetable compiler
       void gen_asm(ostream& o);
       string IR_reg_to_asm(string reg); /**< helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24 */
       void gen_asm_prologue(ostream& o);
       void gen_asm_epilogue(ostream& o);
   
       // symbol table methods
       void add_to_symbol_table(string name, Type t,int line, int col);
       void add_to_symbol_table(string name, Type t, int size = 1);
       void use_variable(string name, int line, int col);
       void define_variable(string name,int line, int col);

       void checkUsageST();
       void store_load_optim();
       string create_new_tempvar(Type t);
       int get_var_index(string name);
       Type get_var_type(string name);
   
       // basic block management
       string new_BB_name();
       BasicBlock* current_bb;
   
    //protected:
       string funcName;
       SymbolTable * symbolTable;
       int nextBBnumber; /**< just for naming */
       int nextTempIndex;
       
       vector <BasicBlock*> bbs; /**< all the basic blocks of this CFG*/
   };
   
#endif