#include "IRVisitor.h"

antlrcpp::Any IRVisitor::visitFunc(ifccParser::ProgContext *ctx) 
{
    CFG * cfg = new CFG(this);
    listCFG->push_back(cfg);
    listCFG->rbegin()->add_to_symbol_table('!reg', int);

    for(ifccParser::StmtContext * i : ctx->stmt()) this->visit( i );
    this->visit( ctx->return_stmt() );

    return 0;
}

// -------------------------------------- RETURN -------------------------------------

antlrcpp::Any IRVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
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

    std::vector<string> params = ['!reg', retval];
    listCFG->rbegin()->current_bb->add_IRInstr(ldconst, int, params);
    return 0;
}

antlrcpp::Any IRVisitor::visitExprvar(ifccParser::ExprvarContext *ctx) {
    std::string retvar = ctx->VAR()->getText();

    std::vector<string> params = ['!reg', retvar];
    listCFG->rbegin()->current_bb->add_IRInstr(copy, int, params);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprbracket(ifccParser::ExprbracketContext *ctx) {
    this->visit( ctx->expr() );
    return 0;
}

antlrcpp::Any IRVisitor::visitExprunaire(ifccParser::ExprunaireContext *ctx) {
    this->visit( ctx->expr() );

    if ( (ctx->UNAIRE()->getText()).compare("!") == 0) {
        std::vector<string> params = ['!reg'];
        listCFG->rbegin()->current_bb->add_IRInstr(notU, int, params);
    } else if (ctx->UNAIRE()->getText().compare("-") == 0 ) {
        std::vector<string> params = ['!reg'];
        listCFG->rbegin()->current_bb->add_IRInstr(negU, int, params);
    }

    return 0;
}


antlrcpp::Any IRVisitor::visitExprmuldivmod(ifccParser::ExprmuldivmodContext *ctx) {
    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[1] );

    // Copy de %eax dans tmp
    std::string tmp = listCFG->rbegin()->create_new_tempvar(int);
    std::vector<string> params = [tmp, '!reg'];
    listCFG->rbegin()->current_bb->add_IRInstr(copy, int, params);

    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[0] );

    // addition ou soustraction : %eax = %eax +- tmp
    if ( (ctx->ADDSUB()->getText()).compare("*") == 0) {
        std::vector<string> params = ['!reg', tmp];
        listCFG->rbegin()->current_bb->add_IRInstr(mul, int, params);

    } else if ( (ctx->ADDSUB()->getText()).compare("/") == 0){
        std::vector<string> params = ['!reg', tmp]; 
        listCFG->rbegin()->current_bb->add_IRInstr(div, int, params);
    } else {
         std::vector<string> params = ['!reg', tmp]; 
        listCFG->rbegin()->current_bb->add_IRInstr(mod, int, params);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcomplg(ifccParser::ExprcomplgContext *ctx) {
    this->visit( ctx->expr()[1] );
    string tmp = bb->cfg->create_new_tempvar(int);

    // met la valeur de %eax dans tmp
    vector<string> params = [tmp, '!reg'];
    listCFG->rbegin()->current_bb->add_IRInstr(copy, int, params);

    this->visit( ctx->expr()[0] );

    vector<string> params = ['!reg', tmp];

    if ( (ctx->COMPLG()->getText()).compare("<") == 0) {
        listCFG->rbegin()->current_bb->add_IRInstr(cmp_lt, int, params);
    } else { 
        listCFG->rbegin()->current_bb->add_IRInstr(cmp_gt, int, params);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcompeqdiff(ifccParser::ExprcompeqdiffContext *ctx) {
    this->visit( ctx->expr()[1] );
    string tmp = bb->cfg->create_new_tempvar(int);

    // met la valeur de %eax dans tmp
    vector<string> params = [tmp, '!reg'];
    listCFG->rbegin()->current_bb->add_IRInstr(copy, int, params);

    this->visit( ctx->expr()[0] );

    vector<string> params = ['!reg', tmp];

    if ( (ctx->COMPEQDIFF()->getText()).compare("==") == 0) {
        listCFG->rbegin()->current_bb->add_IRInstr(cmp_eq, int, params);
    } else { 
        listCFG->rbegin()->current_bb->add_IRInstr(cmp_neq, int, params);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExpraddsub(ifccParser::ExpraddsubContext *ctx) {
    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[1] );

    // Copy de %eax dans tmp
    std::string tmp = listCFG->rbegin()->create_new_tempvar(int);
    std::vector<string> params = [tmp, '!reg'];
    listCFG->rbegin()->current_bb->add_IRInstr(copy, int, params);

    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[0] );

    // addition ou soustraction : %eax = %eax +- tmp
    if ( (ctx->ADDSUB()->getText()).compare("+") == 0) {
        std::vector<string> params = ['!reg', tmp];
        listCFG->rbegin()->current_bb->add_IRInstr(add, int, params);

    } else {
        std::vector<string> params = ['!reg', tmp]; 
        listCFG->rbegin()->current_bb->add_IRInstr(sub, int, params);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprandbb(ifccParser::ExprandbbContext *ctx) {
    this->visit( ctx->expr()[1] );
    string tmp = bb->cfg->create_new_tempvar(int);

    // met la valeur de %eax dans tmp
    vector<string> params = [tmp, '!reg'];
    listCFG->rbegin()->current_bb->add_IRInstr(copy, int, params);

    this->visit( ctx->expr()[0] );

    vector<string> params = ['!reg', tmp];
    listCFG->rbegin()->current_bb->add_IRInstr(andbb, int, params);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprnotbb(ifccParser::ExprnotbbContext *ctx) {
    this->visit( ctx->expr() );
    vector<string> params = ['!reg'];
    listCFG->rbegin()->current_bb->add_IRInstr(notbb, int, params);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprorbb(ifccParser::ExprorbbContext *ctx) {
    this->visit( ctx->expr()[1] );
    string tmp = bb->cfg->create_new_tempvar(int);

    // met la valeur de %eax dans tmp
    vector<string> params = [tmp, '!reg'];
    listCFG->rbegin()->current_bb->add_IRInstr(copy, int, params);

    this->visit( ctx->expr()[0] );

    vector<string> params = ['!reg', tmp];
    listCFG->rbegin()->current_bb->add_IRInstr(orbb, int, params);

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