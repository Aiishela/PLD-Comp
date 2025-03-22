#pragma once
#include <map>
#include <string>

class SymbolTable {
    public:
        SymbolTable();

        struct VariableInfo {
            int index; 
            bool declared;
            bool defined;
            bool used;
        };

        void addVariable(const std::string& name, bool init) ;
        void useVariable(const std::string& name) ;

        int addTempVariable();

        VariableInfo getVariableInfo(const std::string& name) ;
        void changeValueVariable(const std::string& name) ;
        void changeValueVariable(const std::string& name, const std::string& name2) ;
        bool checkVariable(const std::string& name) ;

        void printVariable(const std::string& name) ;

        bool checkUsageST() ;

        std::map<std::string, VariableInfo> *st;

    private:
        int nextIndex; 
        int tempIndex;
};

extern SymbolTable *symbolTable;