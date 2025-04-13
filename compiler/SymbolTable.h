#pragma once
#include <map>
#include <string>
#include "Type.h"

using namespace std;

class SymbolTable {
    public:
        SymbolTable();

        struct VariableInfo {
            int index;
            Type type; 
            bool declared;
            bool defined;
            bool used;
            int scopeLevel;
        };

        // Deep Copy Constructor
        SymbolTable(const SymbolTable &other) {
            st = new std::multimap<std::string, VariableInfo>(*other.st);
        }

        // Deep Copy Assignment Operator
        SymbolTable& operator=(const SymbolTable &other) {
            if (this != &other) {
                // First, delete the current contents of st (if it exists)
                delete st;

                // Perform a deep copy of the multimap
                st = new std::multimap<std::string, VariableInfo>(*other.st);
            }
            return *this;
        }


        // Destructor
        ~SymbolTable() {
            delete st;
        }

        void addVariable(const string& name, Type t, int line, int col) ;
        void addVariable(const string& name, Type t, int size) ;
        void useVariable(const string& name, int line, int col) ;
        void defineVariable(const string& name, int line, int col) ;

        void printError(string text, int line, int col);
        void printWarning(string text, int line, int col);

        int addTempVariable();

        VariableInfo getVariableInfo(const string& name, int scope = -1) ;
        void incrScopeLevel() {currentScopeLevel++;}
        void decrScopeLevel() {currentScopeLevel--;}
        int getCurrentScopeLevel() {return currentScopeLevel;}
        void mergeSymbolTablesRespectingScope(const SymbolTable& inner) ;
        //void changeValueVariable(const string& name) ;
        //void changeValueVariable(const string& name, const string& name2) ;
        bool existVariable(const string& name, int scope = -1) ;

        void printVariable(const string& name) ;
        void printSymbolTable();

        bool checkUsageST() ;
        bool getError() { return error; };

        multimap<string, VariableInfo> *st;

    private:
        int nextIndex; 
        int tempIndex;
        bool error;
        int currentScopeLevel;
};
