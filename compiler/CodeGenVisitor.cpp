#include "CodeGenVisitor.h"
#include "SymbolTable.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    #ifdef __APPLE__
    std::cout<< ".globl _main\n" ;
    std::cout<< " _main: \n" ;
    #else
    std::cout<< ".globl main\n" ;
    std::cout<< " main: \n" ;
    #endif
    std::cout<< "   # prologue\n" ;
    std::cout<< "   pushq %rbp \n" ;
    std::cout<< "   movq %rsp, %rbp\n" ;

    for(ifccParser::StmtContext * i : ctx->stmt()) this->visit( i );
    this->visit( ctx->return_stmt() );

    std::cout<< "   # epilogue\n" ;
    std::cout<< "   popq %rbp\n" ;

    
    std::cout << "   ret\n";

    return 0;
}

// -------------------------------------- RETURN -------------------------------------


antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    this->visit( ctx->expr() );

    return 0;
}

// -------------------------------------- EXPR -------------------------------------

antlrcpp::Any CodeGenVisitor::visitExprconst(ifccParser::ExprconstContext *ctx) {
    int retval = stoi(ctx->CONST()->getText());

    std::cout << "   movl $"<<retval<<", %eax\n" ;

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprvar(ifccParser::ExprvarContext *ctx) {
    std::string var = ctx->VAR()->getText();
    int index = (*symbolTable->st)[var].index;
    std::cout << "   movl "<<index<<"(%rbp), %eax\n" ;

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprmul(ifccParser::ExprmulContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );

    std::cout << "   imull " << tempIndex <<"(%rbp), %eax\n" ; 

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprdiv(ifccParser::ExprdivContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );

    std::cout << "   cltd" << std::endl;
    std::cout << "   idivl " << tempIndex <<"(%rbp)\n" ; // idivl : eax / la dest indiqué, quotient dans eax

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpradd(ifccParser::ExpraddContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );

    std::cout << "   addl " << tempIndex <<"(%rbp), %eax\n" ; 

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprsub(ifccParser::ExprsubContext *ctx) { 
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );

    std::cout << "   subl " << tempIndex <<"(%rbp), %eax\n" ; 

    return 0;
}


// --------------------------------------- DECLARATION --------------------------------

antlrcpp::Any CodeGenVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    this->visit( ctx->decl() );

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclconst(ifccParser::DeclconstContext *ctx) {
    int retval = stoi(ctx->CONST()->getText());
    std::string var = ctx->VAR()->getText();

    std::cout << "   movl $"<<retval<<", "<< (*symbolTable->st)[var].index <<"(%rbp)" << '\n' ;

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclalone(ifccParser::DeclaloneContext *ctx) {

    return 0;
}

// --------------------------------------- AFFFECTATION --------------------------------

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    this->visit( ctx->aff() );

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAff(ifccParser::AffContext *ctx) {
    this->visit( ctx->expr() );

    std::string var = ctx->VAR()->getText();

    std::cout << "   movl %eax, "<< (*symbolTable->st)[var].index <<"(%rbp)" << '\n' ;
    return 0;
}

/*antlrcpp::Any CodeGenVisitor::visitAffconst(ifccParser::AffconstContext *ctx) {
    int retval = stoi(ctx->CONST()->getText());
    std::string var = ctx->VAR()->getText();

    std::cout << "   movl $"<<retval<<", "<< (*symbolTable->st)[var].index <<"(%rbp)" << '\n' ;

    return 0;
}*/

/*antlrcpp::Any CodeGenVisitor::visitAffvar(ifccParser::AffvarContext *ctx) {
    std::string varLeft = ctx->VAR()[0]->getText();
    std::string varRight = ctx->VAR()[1]->getText();

    std::cout << "   movl "<<(*symbolTable->st)[varRight].index <<"(%rbp),  %eax\n";
    std::cout << "   movl %eax, "<<(*symbolTable->st)[varLeft].index <<"(%rbp)\n";

    return 0;
}*/
