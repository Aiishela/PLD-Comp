#pragma once
#include <map>
#include <string>

class SymbolTable {
    public:
        SymbolTable();

        struct VariableInfo {
            int index;  
            int value; 
            bool declared;
            bool defined;
            bool used;
        };

        void addVariable(const std::string& name, int value) ;
        void addVariable(const std::string& name) ;
        int useVariable(const std::string& name) ;

        VariableInfo getVariableInfo(const std::string& name) ;
        void changeValueVariable(const std::string& name, int value) ;
        void changeValueVariable(const std::string& name, const std::string& name2) ;
        bool checkVariable(const std::string& name) ;

        void printVariable(const std::string& name) ;

        bool checkUsageST() ;

        std::map<std::string, VariableInfo> *st;

    private:
        int nextIndex; 
};

extern SymbolTable *symbolTable;