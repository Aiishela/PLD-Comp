#ifndef BASICBLOCK_H
#define BASICBLOCK_H

#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>
#include <map>

#include "Operation.h"
#include "Type.h"

class IRInstr;
class IRInstr;
class CFG;

using namespace std;

/**  Classe pour un BasicBlock */
class BasicBlock
{
	public:
		BasicBlock(CFG *cfg, string entry_label);
		void gen_asm(ostream &o); /** Génération du code assembleur x86 */
		void add_IRInstr(Operation op, Type t, vector<string> params); /** Ajout d'une IRInstr au BasicBlock */
		IRInstr *getLastInstr(); /** Renvoie la dernière instruction de instrs */
		void removeInstrs(int indexBegin, int indexEnd = -1); /**< Pour l'optimisation : Supprime les instructions situées entre les deux index */

		void store_load_optim(); /** Optimization des instructions à la fin du parcours du programme */

		void setExit_true(BasicBlock *bb){exit_true = bb;}
		void setExit_false(BasicBlock *bb){exit_false = bb;}
		void setTest_var_name(string name){test_var_name = name; }
		int getInstrsSize(){return instrs.size();}
		CFG * getCfg() {return cfg;}

		
	protected:
		string test_var_name;	  /** < when generating IR code for an if(expr) or while(expr) etc, store here the name of the variable that holds the value of expr */
		vector<IRInstr *> instrs; /** < the instructions themselves. */
		CFG *cfg;				  /** < the CFG where this block belongs */
		string label;			  /**< label of the BB, also will be the label in the generated code */
		BasicBlock *exit_true;	  /**< pointer to the next basic block, true branch. If nullptr, return from procedure */
		BasicBlock *exit_false;	  /**< pointer to the next basic block, false branch. If null_ptr, the basic block ends with an unconditional jump */
	};

#endif
