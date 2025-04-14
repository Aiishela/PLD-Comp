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
            bool used;
        };

        // Deep Copy Constructor
        SymbolTable(const SymbolTable &other) {
            st = new std::map<std::string, VariableInfo>(*other.st);
        }

        // Deep Copy Assignment Operator
        SymbolTable& operator=(const SymbolTable &other) {
            if (this != &other) {
                delete st;
                st = new std::map<std::string, VariableInfo>(*other.st);
            }
            return *this;
        }

        // Destructor
        ~SymbolTable() {
            delete st;
        }

        
        void addVariable(const string& name, Type t, int line, int col) ;
        void addVariable(const string& name, Type t) ;
        void useVariable(const string& name, int line, int col) ;

        void printError(string text, int line, int col);

        int addTempVariable();

        VariableInfo getVariableInfo(const string& name) ;
        //void changeValueVariable(const string& name) ;
        //void changeValueVariable(const string& name, const string& name2) ;
        bool existVariable(const string& name) ;

        void printVariable(const string& name) ;

        bool checkUsageST() ;
        bool getError() { return error; };

        map<string, VariableInfo> *st;

    private:
        int nextIndex; 
        int tempIndex;
        bool error;
};
