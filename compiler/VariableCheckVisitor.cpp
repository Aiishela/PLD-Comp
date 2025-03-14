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
    this->visit( ctx->expr() );

    return 0;
}

// ------------------------------------------ EXPR -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitExprconst(ifccParser::ExprconstContext *ctx) {
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprvar(ifccParser::ExprvarContext *ctx) {
    std::string var = ctx->VAR()->getText();
    int value = symbolTable->useVariable(var);

    return value;
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

antlrcpp::Any VariableCheckVisitor::visitAff(ifccParser::AffContext *ctx) {
    int value = std::any_cast<int>(this->visit( ctx->expr() ));

    std::string var = ctx->VAR()->getText();

    symbolTable->changeValueVariable(var, value);
 
    return 0;
}


/*antlrcpp::Any VariableCheckVisitor::visitAffconst(ifccParser::AffconstContext *ctx) {
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
}*/
