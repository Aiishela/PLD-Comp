#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTable.h"
#include <map>
#include <string>


class  VariableCheckVisitor : public ifccBaseVisitor {
    public:
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;

        // RETURN 
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitReturnconst(ifccParser::ReturnconstContext *ctx) override ;
        virtual antlrcpp::Any visitReturnvar(ifccParser::ReturnvarContext *ctx) override ;

        // DECLARATION
        virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override ;
        virtual antlrcpp::Any visitDeclconst(ifccParser::DeclconstContext *ctx) override ;    
        virtual antlrcpp::Any visitDeclalone(ifccParser::DeclaloneContext *ctx) override ;

        // AFFECTATION
        virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override ;
        virtual antlrcpp::Any visitAffconst(ifccParser::AffconstContext *ctx) override ;
        virtual antlrcpp::Any visitAffvar(ifccParser::AffvarContext *ctx) override ;

};

