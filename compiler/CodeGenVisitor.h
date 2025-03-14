#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTable.h"

#include <map>
#include <string>


class  CodeGenVisitor : public ifccBaseVisitor {
	public:
                virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;

                // RETURN 
                virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;

                // EXPR
                virtual antlrcpp::Any visitExprconst(ifccParser::ExprconstContext *ctx) override ;
                virtual antlrcpp::Any visitExprvar(ifccParser::ExprvarContext *ctx) override ;
                virtual antlrcpp::Any visitExprmul(ifccParser::ExprmulContext *ctx) override ;
                virtual antlrcpp::Any visitExprdiv(ifccParser::ExprdivContext *ctx) override ;
                virtual antlrcpp::Any visitExpradd(ifccParser::ExpraddContext *ctx) override ;
                virtual antlrcpp::Any visitExprsub(ifccParser::ExprsubContext *ctx) override ;

                // DECLARATION
                virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override ;
                virtual antlrcpp::Any visitDeclconst(ifccParser::DeclconstContext *ctx) override ;
                virtual antlrcpp::Any visitDeclalone(ifccParser::DeclaloneContext *ctx) override ;

                // AFFECTATION
                virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override ;
                virtual antlrcpp::Any visitAff(ifccParser::AffContext *ctx) override ;
                /*virtual antlrcpp::Any visitAffconst(ifccParser::AffconstContext *ctx) override ;
                virtual antlrcpp::Any visitAffvar(ifccParser::AffvarContext *ctx) override ;*/

        /*protected:
                std::map<std::string, int> * symbolTable;*/
};

