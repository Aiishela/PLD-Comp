#include "SymbolTable.h"
#include "Type.h"
#include <iostream>
#include <iomanip>

#define WARNING "\033[01;35m" << "warning: " << "\033[0m"
#define RESET  "\033[0m"
#define ERROR "\033[31m" << "error: " << "\033[0m"

SymbolTable::SymbolTable() {
    this->st = new multimap<string, VariableInfo>;
    this->nextIndex = -4; 
    this->tempIndex = 1;
    this->currentScopeLevel = 0;
}

int SymbolTable::addTempVariable() {
    VariableInfo varInfo = { nextIndex , INT, true, true, true, currentScopeLevel};  
    nextIndex = nextIndex - 4;

    string tempName = "!tmp" + tempIndex;
    tempIndex++;
    st->insert(make_pair(tempName, varInfo));

    return varInfo.index;
}

void SymbolTable::printVariable(const string& name) {
// Algorithme : Affiche la variable : son nom, son index
    /*cout << "Index : " << (*st)[name].index << setw(10)
              << " Name : " << name << endl;*/
}

void SymbolTable::addVariable(const std::string& name, Type t, int line, int col) {
    // Algorithme : Ajoute la variable dans la ST
    //              Si elle est déjà présente dans la table, renvoie un message et ne l'ajoute pas dans la ST
    //              Si une variable du même nom existe dans le même scope, c’est une redéclaration interdite
    auto it = st->find(name);
    if (it != st->end() && it->second.scopeLevel == currentScopeLevel) {
        printError("Variable " + name + " is declared twice in the same scope.", line, col);
        return;
    }

    // Sinon, on peut ajouter la variable (même nom autorisé dans un scope plus profond)
    VariableInfo varInfo = { nextIndex, t, true, false, false, currentScopeLevel };  
    nextIndex -= 4;
    st->insert(make_pair(name, varInfo));
    printVariable(name);
}


void SymbolTable::addVariable(const string& name, Type t, int size) { 
    // Algorithme : Ajoute la variable dans la ST sans faire aucune vérification
    //              Si elle est déjà présente dans la table, renvoie un message et ne l'ajoute pas dans la ST
        cout << "addVariable" << endl;

    VariableInfo varInfo = { nextIndex , t, true, false, false, currentScopeLevel};  
    nextIndex = nextIndex - 4;
    if (size == 1) { // une variable à rajouter
        VariableInfo varInfo = { nextIndex , t, true, false, false, currentScopeLevel};  
        nextIndex = nextIndex - 4;
        st->insert(make_pair(name, varInfo));
    
        printSymbolTable();

    } else { // un tableau à rajouter
        cout << "wheyyeyeyeye" << endl;
        int tempIndex = nextIndex - (size-1) * 4;    // a[1] = a[0] + 4
        nextIndex = nextIndex - size * 4;

        VariableInfo varInfo = { tempIndex , t, true, false, false, currentScopeLevel};  
        tempIndex = tempIndex + 4;
        st->insert(make_pair(name, varInfo));

        for (int i = 1; i < size; i++) {
            VariableInfo varInfo = { tempIndex , t, true, false, false, currentScopeLevel};  
            tempIndex = tempIndex - 4;
            st->insert(make_pair(name + "-" + to_string(i), varInfo));
        }
    }
    
    
    }

void SymbolTable::useVariable(const string& name, int line, int col) {
// Algorithme : Utilise la variable
//              Si la variable n'est pas définie ou déclaré, renvoie un message d'erreur.
    bool present = existVariable(name);

    if (!present) {
        cout << "useVaraible" << name << endl;
        printError("Variable " + name + " is not declared.", line, col);
    }
    else if (present) {
        //(*st)[name].used = true; 
    } else {
        printError("Variable " + name + " is not defined.", line, col);
        
    }

    
}

SymbolTable::VariableInfo SymbolTable::getVariableInfo(const string& varName, int scope) {
    // Iterate through the symbol table to find the variable with the closest scope level
    
    // First, find all entries with the same variable name
    auto range = st->equal_range(varName);
    
    // Iterate over the range of elements with the same name
    for (auto it = range.first; it != range.second; ++it) {
        // Check if the scopeLevel matches
        if (it->second.scopeLevel == scope or scope == -1) {
            return it->second;  // Return the VariableInfo that matches the name and scopeLevel
        }
    }

    // If no variable is found, return a default (empty or invalid) VariableInfo
    cerr << "Error: Variable '" << varName << "' not found in symbol table for the given scope!" << endl;
    return VariableInfo();  // Returning a default constructed VariableInfo (you can customize this if needed)
}

void SymbolTable::defineVariable(const string& name, int line, int col) {
// Algorithme : Définie la variable
    /*if(st->find(name) != st->end()){
        (*st)[name].defined = true;
    } else {
        printError("Variable " + name + " is not declared.", line, col);
    }*/
}

bool SymbolTable::existVariable(const std::string& name, int scope) {
    if (scope !=-1 ) {
        for (int lvl = scope; lvl >= 0; --lvl) {
            for (const auto& entry : *st) {
                if (entry.first == name && entry.second.scopeLevel == lvl) {
                    return true;
                }
            }
        }
    } else {
        return (st->find(name) != st->end());
    }
    return false;
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

void SymbolTable::mergeSymbolTablesRespectingScope(const SymbolTable& inner) {
    for (const auto& [name, innerVar] : *inner.st) {
        auto it = st->find(name);
        cout << name << endl;
        
        if (it == st->end()) {
            cout << "here" << endl;
            // La variable n'existe pas encore : on peut l'ajouter
            VariableInfo newVar = innerVar;
            st->insert(make_pair(name, newVar));
        } else {
            // La variable existe déjà : on garde celle avec le scope le plus global (plus petit scopeLevel)
            VariableInfo& outerVar = it->second;
            cout << outerVar.scopeLevel << endl;
            if (innerVar.scopeLevel < outerVar.scopeLevel) {
                // cas rare : innerVar vient d’un scope plus global ? On écrase (normalement jamais)
            st->insert(make_pair(name, innerVar));
                cout << "impossible" << endl;
            } else if (innerVar.scopeLevel == outerVar.scopeLevel) {
                cout << "equal" << endl;
                // Même niveau : fusion des infos
                outerVar.declared = outerVar.declared && innerVar.declared;
                outerVar.defined = outerVar.defined && innerVar.defined;
                outerVar.used = outerVar.used || innerVar.used;
            }
            // Sinon on garde outerVar (scope plus global)
        }
    }
}


void SymbolTable::printError(string text, int line, int col){
    cout << "line:" << line  <<":" << col  << ": " << ERROR << text << endl;
    error = true;
}

void SymbolTable::printWarning(string text, int line, int col){
    cout << "line:" << line  <<":" << col  << ": " << WARNING << text << endl;
}

void SymbolTable::printSymbolTable() {
    // Loop through all entries in the symbol table
    for (const auto& entry : *st) {
        const string& varName = entry.first;
        const VariableInfo& varInfo = entry.second;

        // Print the variable information
        cout << "Variable Name: " << varName << "\n";
        cout << "Index: " << varInfo.index << "\n";
        cout << "Type: " << (varInfo.type == Type::INT ? "INT" : "CHAR") << "\n";
        cout << "Declared: " << (varInfo.declared ? "Yes" : "No") << "\n";
        cout << "Defined: " << (varInfo.defined ? "Yes" : "No") << "\n";
        cout << "Used: " << (varInfo.used ? "Yes" : "No") << "\n";
        cout << "Scope Level: " << varInfo.scopeLevel << "\n";
        cout << "------------------------------------\n";
    }
}

