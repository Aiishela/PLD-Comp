#pragma once

#include "antlr4-runtime.h"
#include <list>
#include <string>
#include "IR.h"


class  ASTGen {
    public:
        ASTGen() : listCFG(new list<CFG>);
        void ASTGen::createNewCFG();

    protected:
        std::list<CFG> * listCFG;
}
