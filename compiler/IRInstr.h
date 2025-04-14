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

//! Classe pour une instruction
class IRInstr {
 
   public:

	IRInstr(BasicBlock* bb_, Operation op, Type t, vector<string> params);

	int getConstValue(); /** Retourne la valeur constante de l'opération ldconst */
	Operation getOp() { return op;} 
	vector<string> getParams() { return params;} 

	void gen_asm(ostream &o); /** Génération du code assembleur x86  */
	
 private:
	BasicBlock* bb; /**BB auquel appartient cette instruction */
	Operation op;
	Type t;
	vector<string> params; /** Contient les paramètres nécessaires à la génaration du code (nom des variables) */
};

#endif
