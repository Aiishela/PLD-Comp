#ifndef IRINSTR_H
#define IRINSTR_H

#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>
#include <map>

#include "Type.h"
#include "Operation.h"

using namespace std;

class BasicBlock;

//! The class for one 3-address instruction
class IRInstr {
 
   public:

	/**  constructor */
	IRInstr(BasicBlock* bb_, Operation op, Type t, vector<string> params);

	int getConstValue(); /** return the const value in the params of the ldconst operator */
	Operation getOp() { return op;} 
	vector<string> getParams() { return params;} 
	/** Actual code generation */
	void gen_asm(ostream &o); /**< x86 assembly code generation for this IR instruction */
	
 private:
	BasicBlock* bb; /**< The BB this instruction belongs to, which provides a pointer to the CFG this instruction belong to */
	Operation op;
	Type t;
	vector<string> params; /**< For 3-op instrs: d, x, y; for ldconst: d, c;  For call: label, d, params;  for wmem and rmem: choose yourself */
	// if you subclass IRInstr, each IRInstr subclass has its parameters and the previous (very important) comment becomes useless: it would be a better design. 
};

#endif
