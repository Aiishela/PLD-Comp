#include "SymbolTable.h"
#include "Type.h"
#include <iostream>
#include <iomanip>

#define WARNING "\033[01;35m" << "warning: " << "\033[0m"
#define RESET  "\033[0m"
#define ERROR "\033[31m" << "error: " << "\033[0m"

SymbolTable::SymbolTable() {
    this->st = new map<string, VariableInfo>;
    this->nextIndex = -4; 
    this->tempIndex = 1;
}

int SymbolTable::addTempVariable() {
    VariableInfo varInfo = { nextIndex , INT, true, true, true};  
    nextIndex = nextIndex - 4;

    string tempName = "!tmp" + tempIndex;
    tempIndex++;
    (*st)[tempName] = varInfo;  

    return varInfo.index;
}

void SymbolTable::printVariable(const string& name) {
// Algorithme : Affiche la variable : son nom, son index
    /*cout << "Index : " << (*st)[name].index << setw(10)
              << " Name : " << name << endl;*/
}

void SymbolTable::addVariable(const string& name, Type t, int line, int col) { 
// Algorithme : Ajoute la variable dans la ST
//              Si elle est déjà présente dans la table, renvoie un message et ne l'ajoute pas dans la ST
    bool present = existVariable(name);

    if (!present) {
        VariableInfo varInfo = { nextIndex , t, true, false, false};  
        nextIndex = nextIndex - 4;
        (*st)[name] = varInfo;  
        printVariable(name);
    } else {
        printError("Variable " + name + " is declared twice.", line, col);
    }

}

void SymbolTable::addVariable(const string& name, Type t, int size) { 
    // Algorithme : Ajoute la variable dans la ST sans faire aucune vérification
    //              Si elle est déjà présente dans la table, renvoie un message et ne l'ajoute pas dans la ST
    VariableInfo varInfo = { nextIndex , t, true, false, false};  
    nextIndex = nextIndex - 4;
    if (size == 1) { // une variable à rajouter
        VariableInfo varInfo = { nextIndex , t, true, false, false};  
        nextIndex = nextIndex - 4;
        (*st)[name] = varInfo;

    } else { // un tableau à rajouter
        int tempIndex = nextIndex - (size-1) * 4;    // a[1] = a[0] + 4
        nextIndex = nextIndex - size * 4;

        VariableInfo varInfo = { tempIndex , t, true, false, false};  
        tempIndex = tempIndex + 4;
        (*st)[name] = varInfo;

        for (int i = 1; i < size; i++) {
            VariableInfo varInfo = { tempIndex , t, true, false, false};  
            tempIndex = tempIndex - 4;
            (*st)[name + "-" + to_string(i)] = varInfo;
        }
    }
    
    
    }

void SymbolTable::useVariable(const string& name, int line, int col) {
// Algorithme : Utilise la variable
//              Si la variable n'est pas définie ou déclaré, renvoie un message d'erreur.
    bool present = existVariable(name);

    if (!present) {
        printError("Variable " + name + " is not declared.", line, col);
    }
    else if (present && (*st)[name].defined) {
        (*st)[name].used = true; 
    } else {
        printError("Variable " + name + " is not defined.", line, col);
        
    }

    
}

SymbolTable::VariableInfo SymbolTable::getVariableInfo(const string& varName) {
// Algorithme : Renvoie les informations à propos d'une variable
    return st->at(varName);
}

void SymbolTable::defineVariable(const string& name, int line, int col) {
// Algorithme : Définie la variable
    if(st->find(name) != st->end()){
        (*st)[name].defined = true;
    } else {
        printError("Variable " + name + " is not declared.", line, col);
    }
}

/*void SymbolTable::changeValueVariable(const string& name, const string& name2) {
// Algorithme : Change la valeur d'une variable par la valeur d'une autre variable
//              Si la variable gauche n'est pas déclarée, renvoie un message d'erreur
//              Si la variable droite n'est pas déclarée ni définie, renvoie un message d'erreur

    bool present = existVariable(name);
    bool possible = true;
    if (!present) {
        cout << "Variable " << name << " is not declared." << endl;
        possible = false;
    }

    present = existVariable(name2);
    if (!present) {
        cout << "Variable " << name2 << " is not declared." << endl;
        possible = false;
    } else if ((*st)[name2].defined == false) {
        cout << "Variable " << name2 << " is not defined, define it before using it in an affectation." << endl;
        possible = false;
    }

    if (possible) {
        (*st)[name2].used = true;
        (*st)[name].defined = true;
    }

}*/

bool SymbolTable::existVariable(const string& name) {
// Algorithme : Vérifie si la variable est présente dans la ST : renvoie true si présente, false sinon
    return (st->find(name) != st->end());
}

bool SymbolTable::checkUsageST() {
// Algorithme : Vérifie si toutes les variables stockées dans la ST ont au moins été utilisées une fois
    bool used = true;
    for (auto it = st->begin(); it != st->end(); it++) {
        if (it->second.used == false ) {
            used = false;
            cout << WARNING <<  "Variable " << it->first << " was not used." << endl;
            //error = true;
        }
    }

    return used;
}

void SymbolTable::printError(string text, int line, int col){
    cout << "line:" << line  <<":" << col  << ": " << ERROR << text << endl;
    error = true;
}

void SymbolTable::printWarning(string text, int line, int col){
    cout << "line:" << line  <<":" << col  << ": " << WARNING << text << endl;
}
