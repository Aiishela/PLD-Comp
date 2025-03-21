#include "IRVisitor.h"

antlrcpp::Any IRVisitor::visitFunc(ifccParser::FuncContext *ctx) 
{
    CFG * cfg = new CFG();
    listCFG->push_back(cfg);
    (*listCFG->rbegin())->add_to_symbol_table("!reg", INT);

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

    std::vector<string> params{"!reg", std::to_string(retval)};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::IRInstr::ldconst, INT, params);
    return 0;
}

antlrcpp::Any IRVisitor::visitExprvar(ifccParser::ExprvarContext *ctx) {
    std::string retvar = ctx->VAR()->getText();

    std::vector<string> params{"!reg", retvar};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprbracket(ifccParser::ExprbracketContext *ctx) {
    this->visit( ctx->expr() );
    return 0;
}

antlrcpp::Any IRVisitor::visitExprunaire(ifccParser::ExprunaireContext *ctx) {
    this->visit( ctx->expr() );

    if ( (ctx->UNAIRE()->getText()).compare("!") == 0) {
        std::vector<string> params{"!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::notU, INT, params);
    } else if (ctx->UNAIRE()->getText().compare("-") == 0 ) {
        std::vector<string> params{"!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::negU, INT, params);
    }

    return 0;
}


antlrcpp::Any IRVisitor::visitExprmuldivmod(ifccParser::ExprmuldivmodContext *ctx) {
    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[1] );

    // Copy de %eax dans tmp
    std::string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);
    std::vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params);

    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[0] );

    // addition ou soustraction : %eax = %eax +- tmp
    if ( (ctx->MULDIVMOD()->getText()).compare("*") == 0) {
        std::vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::mul, INT, params);

    } else if ( (ctx->MULDIVMOD()->getText()).compare("/") == 0){
        std::vector<string> params{"!reg", tmp}; 
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::div, INT, params);
    } else {
         std::vector<string> params{"!reg", tmp}; 
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::mod, INT, params);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcomplg(ifccParser::ExprcomplgContext *ctx) {
    this->visit( ctx->expr()[1] );
    std::string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params);

    this->visit( ctx->expr()[0] );

    vector<string> params2{"!reg", tmp};

    if ( (ctx->COMPLG()->getText()).compare("<") == 0) {
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::cmp_lt, INT, params2);
    } else { 
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::cmp_gt, INT, params2);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcompeqdiff(ifccParser::ExprcompeqdiffContext *ctx) {
    this->visit( ctx->expr()[1] );
    std::string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params);

    this->visit( ctx->expr()[0] );

    vector<string> params2{"!reg", tmp};

    if ( (ctx->COMPEQDIFF()->getText()).compare("==") == 0) {
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::cmp_eq, INT, params2);
    } else { 
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::cmp_neq, INT, params2);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExpraddsub(ifccParser::ExpraddsubContext *ctx) {
    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[1] );

    // Copy de %eax dans tmp
    std::string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);
    std::vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params);

    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[0] );

    // addition ou soustraction : %eax = %eax +- tmp
    if ( (ctx->ADDSUB()->getText()).compare("+") == 0) {
        std::vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::add, INT, params);

    } else {
        std::vector<string> params{"!reg", tmp}; 
        (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::sub, INT, params);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprandbb(ifccParser::ExprandbbContext *ctx) {
    this->visit( ctx->expr()[1] );
    std::string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params);

    this->visit( ctx->expr()[0] );

    vector<string> params2{"!reg", tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::andbb, INT, params2);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprnotbb(ifccParser::ExprnotbbContext *ctx) {
    this->visit( ctx->expr() );
    vector<string> params{"!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::notbb, INT, params);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprorbb(ifccParser::ExprorbbContext *ctx) {
    this->visit( ctx->expr()[1] );
    std::string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params);

    this->visit( ctx->expr()[0] );

    vector<string> params2{"!reg", tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::orbb, INT, params2);

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

    std::vector<string> params{"!reg", std::to_string(retval)};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::IRInstr::ldconst, INT, params);

    std::vector<string> params2{var, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params2);

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

    std::vector<string> params{var, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(IRInstr::copy, INT, params);

    return 0;
}