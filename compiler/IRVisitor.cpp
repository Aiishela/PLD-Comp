#include "IRVisitor.h"

antlrcpp::Any IRVisitor::visitFunc(ifccParser::ProgContext *ctx) 
{
    astgen->createNewCFG();
    for(ifccParser::StmtContext * i : ctx->stmt()) this->visit( i );
    this->visit( ctx->return_stmt() );

    return 0;
}

// -------------------------------------- RETURN -------------------------------------


antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    this->visit( ctx->expr() );

    return 0;
}

// -------------------------------------- EXPR -------------------------------------

antlrcpp::Any IRVisitor::visitExprconst(ifccParser::ExprconstContext *ctx) {
    int retval = stoi(ctx->CONST()->getText());

    std::cout << "   movl $"<<retval<<", %eax\n" ;

    return 0;
}

antlrcpp::Any IRVisitor::visitExprvar(ifccParser::ExprvarContext *ctx) {
    std::string var = ctx->VAR()->getText();
    int index = (*symbolTable->st)[var].index;
    std::cout << "   movl "<<index<<"(%rbp), %eax\n" ;

    return 0;
}

antlrcpp::Any IRVisitor::visitExprbracket(ifccParser::ExprbracketContext *ctx) {
    this->visit( ctx->expr() );
    return 0;
}

antlrcpp::Any IRVisitor::visitExprunaire(ifccParser::ExprunaireContext *ctx) {
    this->visit( ctx->expr() );

    if ( (ctx->UNAIRE()->getText()).compare("!") == 0) {
        std::cout << "   cmpl $0, %eax\n";      // compare la var, const ou l'expression booléene à 0
        std::cout << "   sete %al\n";
        std::cout << "   movzbl %al, %eax\n";
    } else if (ctx->UNAIRE()->getText().compare("-") == 0 ) {
        std::cout << "   negl %eax\n";
    }

    return 0;
}



antlrcpp::Any IRVisitor::visitExprmuldivmod(ifccParser::ExprmuldivmodContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );

    if ( (ctx->MULDIVMOD()->getText()).compare("*") == 0) {
        std::cout << "   imull " << tempIndex <<"(%rbp), %eax\n" ; 
    } else if (ctx->MULDIVMOD()->getText().compare("/") == 0 ){
        std::cout << "   cltd" << std::endl;
        std::cout << "   idivl " << tempIndex <<"(%rbp)\n" ; // idivl : eax / la dest indiqué, quotient dans eax, reste dans edx
    } else { // modulo
        std::cout << "   cltd" << std::endl;
        std::cout << "   idivl " << tempIndex <<"(%rbp)\n" ;
        std::cout << "   movl %edx, %eax \n" ; 
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcomplg(ifccParser::ExprcomplgContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );
    std::cout << "   cmp %eax, " << tempIndex <<"(%rbp)\n" ; // compare gauche > droite 

    if ( (ctx->COMPLG()->getText()).compare("<") == 0) { 
        std::cout << "   setl %al\n" ; 
    } else { 
        std::cout << "   setg %al\n" ; 
    }
    std::cout << "   movzbl	%al, %eax\n" ; 

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcompeqdiff(ifccParser::ExprcompeqdiffContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );

    std::cout << "   cmp %eax, " << tempIndex <<"(%rbp)\n" ; // compare gauche < droite 

    if ( (ctx->COMPEQDIFF()->getText()).compare("==") == 0) {
        std::cout << "   sete %al\n" ; 
    } else { 
        std::cout << "   setne %al\n" ; 
    }
    std::cout << "   movzbl	%al, %eax\n" ; 

    return 0;
}

antlrcpp::Any IRVisitor::visitExpraddsub(ifccParser::ExpraddsubContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );

    if ( (ctx->ADDSUB()->getText()).compare("+") == 0) {
        std::cout << "   addl " << tempIndex <<"(%rbp), %eax\n" ; 
    } else {
        std::cout << "   subl " << tempIndex <<"(%rbp), %eax\n" ; 
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprandbb(ifccParser::ExprandbbContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );
    
    std::cout << "   andl " << tempIndex <<"(%rbp), %eax\n" ; 

    return 0;
}

antlrcpp::Any IRVisitor::visitExprnotbb(ifccParser::ExprnotbbContext *ctx) {
    this->visit( ctx->expr() );
    
    std::cout << "   notl %eax\n" ; 

    return 0;
}

antlrcpp::Any IRVisitor::visitExprorbb(ifccParser::ExprorbbContext *ctx) {
    this->visit( ctx->expr()[1] );

    int tempIndex = symbolTable->addTempVariable();
    std::cout << "   movl %eax, " << tempIndex <<"(%rbp)\n" ; 

    this->visit( ctx->expr()[0] );
    
    std::cout << "   orl " << tempIndex <<"(%rbp), %eax\n" ; 

    return 0;
}


// --------------------------------------- DECLARATION --------------------------------

antlrcpp::Any IRVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    this->visit( ctx->decl() );

    return 0;
}

antlrcpp::Any IRVisitor::visitDeclconst(ifccParser::DeclconstContext *ctx) {
    int retval = stoi(ctx->CONST()->getText());
    std::string var = ctx->VAR()->getText();

    std::cout << "   movl $"<<retval<<", "<< (*symbolTable->st)[var].index <<"(%rbp)" << '\n' ;

    return 0;
}

antlrcpp::Any IRVisitor::visitDeclalone(ifccParser::DeclaloneContext *ctx) {

    return 0;
}

// --------------------------------------- AFFFECTATION --------------------------------

antlrcpp::Any IRVisitor::visitAffectation(ifccParser::AffectationContext *ctx) {
    this->visit( ctx->aff() );

    return 0;
}

antlrcpp::Any IRVisitor::visitAff(ifccParser::AffContext *ctx) {
    this->visit( ctx->expr() );

    std::string var = ctx->VAR()->getText();

    std::cout << "   movl %eax, "<< (*symbolTable->st)[var].index <<"(%rbp)" << '\n' ;
    return 0;
}
