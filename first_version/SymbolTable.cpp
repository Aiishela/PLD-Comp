#include "SymbolTable.h"
#include <iostream>
#include <iomanip>

SymbolTable::SymbolTable() {
    this->st = new std::map<std::string, VariableInfo>;
    this->nextIndex = -4; 
    this->tempIndex = 1;
}

int SymbolTable::addTempVariable() {
    VariableInfo varInfo = { nextIndex , true, true, true};  
    nextIndex = nextIndex - 4;

    std::string tempName = "!tmp" + tempIndex;
    tempIndex++;
    (*st)[tempName] = varInfo;  

    return varInfo.index;
}

void SymbolTable::printVariable(const std::string& name) {
// Algorithme : Affiche la variable : son nom, son index
    std::cout << "Index : " << (*st)[name].index << std::setw(10)
              << " Name : " << name << std::endl;
}

void SymbolTable::addVariable(const std::string& name, bool init) { 
// Algorithme : Ajoute la variable dans la ST
//              Si elle est déjà présente dans la table, renvoie un message et ne l'ajoute pas dans la ST
    bool present = checkVariable(name);

    if (!present) {
        VariableInfo varInfo = { nextIndex , true, init, false};  
        nextIndex = nextIndex - 4;
        (*st)[name] = varInfo;  
        printVariable(name);
    } else {
        std::cout << "Variable " << name << " is declared twice." << std::endl; // afficher ligne et colonne
    }

}

void SymbolTable::useVariable(const std::string& name) {
// Algorithme : Utilise la variable
//              Si la variable n'est pas définie ou déclaré, renvoie un message d'erreur.
    bool present = checkVariable(name);

    if (present && (*st)[name].defined) {
        (*st)[name].used = true; 
    } else {
        std::cout << "Variable " << name << " is not declared." << std::endl; // ligne et colonne
    }
    
}


SymbolTable::VariableInfo SymbolTable::getVariableInfo(const std::string& varName) {
// Algorithme : Renvoie les informations à propos d'une variable
    return st->at(varName);
}

void SymbolTable::changeValueVariable(const std::string& name) {
// Algorithme : Définie la variable
    if(st->find(name) != st->end()){
        (*st)[name].defined = true;
        printVariable(name);
    } else {
        std::cout << "Variable " << name << " is not declared." << std::endl; // ligne et colonne
    }
}

void SymbolTable::changeValueVariable(const std::string& name, const std::string& name2) {
// Algorithme : Change la valeur d'une variable par la valeur d'une autre variable
//              Si la variable gauche n'est pas déclarée, renvoie un message d'erreur
//              Si la variable droite n'est pas déclarée ni définie, renvoie un message d'erreur

    bool present = checkVariable(name);
    bool possible = true;
    if (!present) {
        std::cout << "Variable " << name << " is not declared." << std::endl;
        possible = false;
    }

    present = checkVariable(name2);
    if (!present) {
        std::cout << "Variable " << name2 << " is not declared." << std::endl;
        possible = false;
    } else if ((*st)[name2].defined == false) {
        std::cout << "Variable " << name2 << " is not defined, define it before using it in an affectation." << std::endl;
        possible = false;
    }

    if (possible) {
        (*st)[name2].used = true;
        (*st)[name].defined = true;
        printVariable(name);
    }

}

bool SymbolTable::checkVariable(const std::string& name) {
// Algorithme : Vérifie si la variable est présente dans la ST : renvoie true si présente, false sinon
    return (st->find(name) != st->end());
}

bool SymbolTable::checkUsageST() {
// Algorithme : Vérifie si toutes les variables stockées dans la ST ont au moins été utilisées une fois
    bool used = true;
    for (auto it = st->begin(); it != st->end(); it++) {
        if (it->second.used == false ) {
            used = false;
            std::cout << "Variable " << it->first << " was not used." << std::endl;
        }
    }

    return used;
}

SymbolTable* symbolTable = nullptr;