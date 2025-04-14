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

/** Classe pour le CFG d'une fonction du programme */
class CFG
{
public:
	CFG(string name);

	void add_bb(BasicBlock *bb); /** Ajoute un bb à la suite */

	// Génération du code assembleur en x86
	void gen_asm(ostream &o);
	void gen_asm_prologue(ostream &o);
	void gen_asm_epilogue(ostream &o);

	// Méthode de la table des symboles
	void add_to_symbol_table(string name, Type t, int size = 1); /** Ajout d'un symbole dans la table des symboles, sans vérifications */
	void checkUsageST(); /** Vérifie que toutes les variables définies ont été utilisées */
	void store_load_optim(); /** Optimisation des instructions présentes dans les bbs */
	string create_new_tempvar(Type t); /** Crée une variable temporaire avec un nom unique */

	int get_var_index(string name); /** Renvoie l'index de la variable en paramètre (e.g. -24) */
	Type get_var_type(string name); /** Renvoie le type de la variable en paramètre */

	// Management du BB
	string new_BB_name(); /** Renvoie un nom de BB unique */
	BasicBlock *current_bb; 

	// GETTERS ET SETTERS
	string getFuncName(){return funcName;}
	SymbolTable * getSymbolTable(){return symbolTable;}	
	
	protected:
		vector<BasicBlock *> bbs; /**< all the basic blocks of this CFG*/
		int nextTempIndex;
		int nextBBnumber; /**< just for naming */
		SymbolTable *symbolTable;
		string funcName;

};

#endif