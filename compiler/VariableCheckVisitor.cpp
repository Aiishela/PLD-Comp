#include "VariableCheckVisitor.h"
extern SymbolTable * symbolTable;

antlrcpp::Any VariableCheckVisitor::visitFunc(ifccParser::FuncContext *ctx) 
{
    CFG * cfg = new CFG(ctx->VAR()->getText());
    listCFG->push_back(cfg);

    this->visit( ctx->bloc() );

    return 0;
}

// ------------------------------------------ RETURN -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitReturn(ifccParser::ReturnContext *ctx)
{
    this->visit( ctx->expr() );

    return 0;
}

// ------------------------------------------ EXPR -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitExprconst(ifccParser::ExprconstContext *ctx) {
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprvar(ifccParser::ExprvarContext *ctx) {
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();
    std::string var = ctx->VAR()->getText();
    (*listCFG->rbegin())->symbolTable->useVariable(var, line, col);

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprchar(ifccParser::ExprcharContext *ctx) {
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprbracket(ifccParser::ExprbracketContext *ctx) {
    this->visit( ctx->expr() );
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprpostfix(ifccParser::ExprpostfixContext *ctx) {
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();
    std::string var = ctx->VAR()->getText();
    (*listCFG->rbegin())->symbolTable->useVariable(var, line, col);
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprprefix(ifccParser::ExprprefixContext *ctx) {
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();
    std::string var = ctx->VAR()->getText();
    (*listCFG->rbegin())->symbolTable->useVariable(var, line, col);
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprunaire(ifccParser::ExprunaireContext *ctx) {
    this->visit( ctx->expr() );
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprmuldivmod(ifccParser::ExprmuldivmodContext *ctx)  {
    this->visit( ctx->expr()[0] );
    this->visit( ctx->expr()[1] );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprcomplg(ifccParser::ExprcomplgContext *ctx) {
    this->visit( ctx->expr()[0] );
    this->visit( ctx->expr()[1] );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprcompeqdiff(ifccParser::ExprcompeqdiffContext *ctx) {
    this->visit( ctx->expr()[0] );
    this->visit( ctx->expr()[1] );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExpraddsub(ifccParser::ExpraddsubContext *ctx) {
    this->visit( ctx->expr()[0] );
    this->visit( ctx->expr()[1] ); 

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprandbb(ifccParser::ExprandbbContext *ctx) {
    this->visit( ctx->expr()[0] );
    this->visit( ctx->expr()[1] ); 

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprnotbb(ifccParser::ExprnotbbContext *ctx) {
    this->visit( ctx->expr() ); 

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprorbb(ifccParser::ExprorbbContext *ctx) {
    this->visit( ctx->expr()[0] );
    this->visit( ctx->expr()[1] ); 

    return 0;
}

// ------------------------------------------ DECLARATION -------------------------------------

antlrcpp::Any VariableCheckVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    this->visit( ctx->decl() );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitDeclexpr(ifccParser::DeclexprContext *ctx) { 
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();

    this->visit( ctx->expr() );
    std::string var = ctx->VAR()->getText();

    (*listCFG->rbegin())->symbolTable->addVariable(var, INT, line, col);
    (*listCFG->rbegin())->symbolTable->defineVariable(var, line, col);

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitDeclchar(ifccParser::DeclcharContext *ctx) {
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();
    string var = ctx->VAR()->getText();
    
    (*listCFG->rbegin())->symbolTable->addVariable(var, CHAR, line, col);
    (*listCFG->rbegin())->symbolTable->defineVariable(var, line, col);

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitDeclalone(ifccParser::DeclaloneContext *ctx) { 
    Type t;
    if ( (ctx->type->getText()).compare("int") == 0) {
        t = INT;

    } else {
        t = CHAR;
    }

    for(antlr4::tree::TerminalNode * i : ctx->VAR()) {
        int line = ctx->getStart()->getLine();
        int col = ctx->getStart()->getCharPositionInLine();
        std::string var = i->getText();
        (*listCFG->rbegin())->symbolTable->addVariable(var,t, line, col);
    }

    return 0;
}

// --------------------------------------- AFFFECTATION --------------------------------

antlrcpp::Any VariableCheckVisitor::visitExpression(ifccParser::ExpressionContext *ctx) {
    this->visit( ctx->expr() );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExpraff(ifccParser::ExpraffContext *ctx) {
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();
    std::any_cast<int>(this->visit( ctx->expr() ));
    std::string var = ctx->VAR()->getText();

    (*listCFG->rbegin())->symbolTable->defineVariable(var, line, col);
    // += est ce utilisé?
 
    return 0;
}
