#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTable.h"
#include <map>
#include <string>
#include <list>
#include "CFG.h"

using namespace std;


class  VariableCheckVisitor : public ifccBaseVisitor {
    public:
        VariableCheckVisitor() : listCFG(new list<CFG *>) {}

        virtual antlrcpp::Any visitFunc(ifccParser::FuncContext *ctx) override ; 
        
        // STRUCTURES DE CONTROLE
        virtual antlrcpp::Any visitIfstmt(ifccParser::IfstmtContext *ctx) override;
        virtual antlrcpp::Any visitWhilestmt(ifccParser::WhilestmtContext *ctx) override;

        // EXPR
        virtual antlrcpp::Any visitExprconst(ifccParser::ExprconstContext *ctx) override ;
        virtual antlrcpp::Any visitExprvar(ifccParser::ExprvarContext *ctx) override ;
        virtual antlrcpp::Any visitExprchar(ifccParser::ExprcharContext *ctx) override ; 
        virtual antlrcpp::Any visitExprunaire(ifccParser::ExprunaireContext *ctx) override ;
        virtual antlrcpp::Any visitExprbracket(ifccParser::ExprbracketContext *ctx) override ;
        virtual antlrcpp::Any visitExprpostfix(ifccParser::ExprpostfixContext *ctx) override ;
        virtual antlrcpp::Any visitExprprefix(ifccParser::ExprprefixContext *ctx) override ;
        virtual antlrcpp::Any visitExprmuldivmod(ifccParser::ExprmuldivmodContext *ctx) override ;
        virtual antlrcpp::Any visitExprcomplg(ifccParser::ExprcomplgContext *ctx) override ;
        virtual antlrcpp::Any visitExprcompeqdiff(ifccParser::ExprcompeqdiffContext *ctx) override ;
        virtual antlrcpp::Any visitExpraddsub(ifccParser::ExpraddsubContext *ctx) override ;
        virtual antlrcpp::Any visitExprandbb(ifccParser::ExprandbbContext *ctx) override ;
        virtual antlrcpp::Any visitExprnotbb(ifccParser::ExprnotbbContext *ctx) override ;
        virtual antlrcpp::Any visitExprorbb(ifccParser::ExprorbbContext *ctx) override ;

        // DECLARATION
        virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override ;
        virtual antlrcpp::Any visitDeclexpr(ifccParser::DeclexprContext *ctx) override ;  
        virtual antlrcpp::Any visitDeclchar(ifccParser::DeclcharContext *ctx) override ;  
        virtual antlrcpp::Any visitDeclalone(ifccParser::DeclaloneContext *ctx) override ;

        // AFFECTATION
        virtual antlrcpp::Any visitExpression(ifccParser::ExpressionContext *ctx) override ; 
        virtual antlrcpp::Any visitExpraff(ifccParser::ExpraffContext *ctx) override ; 

        //BLOC
        virtual antlrcpp::Any visitBloc(ifccParser::BlocContext *ctx) override ;
        // RETURN
        virtual antlrcpp::Any visitReturn(ifccParser::ReturnContext *ctx) override;
    
        // APPEL FONCTION
        virtual antlrcpp::Any visitCallfunc(ifccParser::CallfuncContext *ctx) override;

    // protected:
        list<CFG *> * listCFG;

};

