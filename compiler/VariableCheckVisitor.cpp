#include "VariableCheckVisitor.h"
extern SymbolTable * symbolTable;

antlrcpp::Any VariableCheckVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    for(ifccParser::StmtContext * i : ctx->stmt()) this->visit( i );
    this->visit( ctx->return_stmt() );

    return 0;
}

// ------------------------------------------ RETURN -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    this->visit( ctx->return_expr() );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitReturnconst(ifccParser::ReturnconstContext *ctx) {

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitReturnvar(ifccParser::ReturnvarContext *ctx) {
    std::string var = ctx->VAR()->getText();
    symbolTable->useVariable(var);

    return 0;
}

// ------------------------------------------ DECLARATION -------------------------------------

antlrcpp::Any VariableCheckVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    this->visit( ctx->decl() );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitDeclconst(ifccParser::DeclconstContext *ctx) { 
    int retval = stoi(ctx->CONST()->getText());
    std::string var = ctx->VAR()->getText();

    symbolTable->addVariable(var, retval);

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitDeclalone(ifccParser::DeclaloneContext *ctx) { 
    for(antlr4::tree::TerminalNode * i : ctx->VAR()) {
        std::string var = i->getText();
        symbolTable->addVariable(var);
    }

    return 0;
}

// --------------------------------------- AFFFECTATION --------------------------------

antlrcpp::Any VariableCheckVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    this->visit( ctx->aff() );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitAffconst(ifccParser::AffconstContext *ctx) {
    int retval = stoi(ctx->CONST()->getText());
    std::string var = ctx->VAR()->getText();

    symbolTable->changeValueVariable(var, retval);

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitAffvar(ifccParser::AffvarContext *ctx) {
    std::string varLeft = ctx->VAR()[0]->getText();
    std::string varRight = ctx->VAR()[1]->getText();

    symbolTable->changeValueVariable(varLeft, varRight);

    return 0;
}
