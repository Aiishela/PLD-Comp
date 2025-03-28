#include "IRVisitor.h"

antlrcpp::Any IRVisitor::visitFunc(ifccParser::FuncContext *ctx) 
{
    CFG * cfg = new CFG(ctx->VAR()->getText());
    listCFG->push_back(cfg);
    //(*listCFG->rbegin())->add_to_symbol_table("!reg", INT); pas la peine parceque !reg = eax

    for(ifccParser::StmtContext * i : ctx->stmt()) this->visit( i );

    return 0;
}

// -------------------------------------- EXPR -------------------------------------

antlrcpp::Any IRVisitor::visitExprconst(ifccParser::ExprconstContext *ctx) {
    int retval = stoi(ctx->CONST()->getText());

    vector<string> params{"!reg", to_string(retval)};
    (*listCFG->rbegin())->current_bb->add_IRInstr(ldconst, INT, params);
    return 0;
}

antlrcpp::Any IRVisitor::visitExprvar(ifccParser::ExprvarContext *ctx) {
    string retvar = ctx->VAR()->getText();

    vector<string> params{"!reg", retvar};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprchar(ifccParser::ExprcharContext *ctx) {
    char c = ctx->CHAR->getText()[0];
    int retval = c;

    vector<string> params{"!reg", to_string(retval)};
    (*listCFG->rbegin())->current_bb->add_IRInstr(ldconst, CHAR, params);

    return 0;
}


antlrcpp::Any IRVisitor::visitExprbracket(ifccParser::ExprbracketContext *ctx) {
    this->visit( ctx->expr() );
    return 0;
}

antlrcpp::Any IRVisitor::visitExprunaire(ifccParser::ExprunaireContext *ctx) {
    this->visit( ctx->expr() );

    if ( (ctx->unaire->getText()).compare("!") == 0) {
        vector<string> params{"!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(notU, INT, params);
    } else if (ctx->unaire->getText().compare("-") == 0 ) {
        vector<string> params{"!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(negU, INT, params);
    }

    return 0;
}


antlrcpp::Any IRVisitor::visitExprmuldivmod(ifccParser::ExprmuldivmodContext *ctx) {
    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[1] );

    // Operation::copy de %eax dans tmp
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[0] );

    // addition ou soustraction : %eax = %eax +- tmp
    if ( (ctx->MULDIVMOD()->getText()).compare("*") == 0) {
        vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(mul, INT, params);

    } else if ( (ctx->MULDIVMOD()->getText()).compare("/") == 0){
        vector<string> params{"!reg", tmp}; 
        (*listCFG->rbegin())->current_bb->add_IRInstr(div_, INT, params);
    } else {
         vector<string> params{"!reg", tmp}; 
        (*listCFG->rbegin())->current_bb->add_IRInstr(mod, INT, params);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcomplg(ifccParser::ExprcomplgContext *ctx) {
    this->visit( ctx->expr()[1] );
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    this->visit( ctx->expr()[0] );

    vector<string> params2{"!reg", tmp};

    if ( (ctx->COMPLG()->getText()).compare("<") == 0) {
        (*listCFG->rbegin())->current_bb->add_IRInstr(cmp_lt, INT, params2);
    } else { 
        (*listCFG->rbegin())->current_bb->add_IRInstr(cmp_gt, INT, params2);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcompeqdiff(ifccParser::ExprcompeqdiffContext *ctx) {
    this->visit( ctx->expr()[1] );
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    this->visit( ctx->expr()[0] );

    vector<string> params2{"!reg", tmp};

    if ( (ctx->COMPEQDIFF()->getText()).compare("==") == 0) {
        (*listCFG->rbegin())->current_bb->add_IRInstr(cmp_eq, INT, params2);
    } else { 
        (*listCFG->rbegin())->current_bb->add_IRInstr(cmp_neq, INT, params2);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExpraddsub(ifccParser::ExpraddsubContext *ctx) {
    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[1] );

    // Operation::copy de %eax dans tmp
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[0] );

    // addition ou soustraction : %eax = %eax +- tmp
    if ( (ctx->addsub->getText()).compare("+") == 0) {
        vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(add, INT, params);

    } else {
        vector<string> params{"!reg", tmp}; 
        (*listCFG->rbegin())->current_bb->add_IRInstr(sub, INT, params);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprandbb(ifccParser::ExprandbbContext *ctx) {
    this->visit( ctx->expr()[1] );
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    this->visit( ctx->expr()[0] );

    vector<string> params2{"!reg", tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(andbb, INT, params2);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprnotbb(ifccParser::ExprnotbbContext *ctx) {
    this->visit( ctx->expr() );
    vector<string> params{"!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(notbb, INT, params);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprorbb(ifccParser::ExprorbbContext *ctx) {
    this->visit( ctx->expr()[1] );
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    this->visit( ctx->expr()[0] );

    vector<string> params2{"!reg", tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(orbb, INT, params2);

    return 0;
}


// --------------------------------------- DECLARATION --------------------------------

antlrcpp::Any IRVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    this->visit( ctx->decl() );

    return 0;
}

antlrcpp::Any IRVisitor::visitDeclexpr(ifccParser::DeclexprContext *ctx) {
    this->visit( ctx->expr() );
    string var = ctx->VAR()->getText();

    (*listCFG->rbegin())->add_to_symbol_table(var, INT);

    vector<string> params{var, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    return 0;
}

antlrcpp::Any IRVisitor::visitDeclchar(ifccParser::DeclcharContext *ctx) {
    string var = ctx->VAR()->getText();
    char c = ctx->CHAR->getText()[0];
    int retval = c;
    (*listCFG->rbegin())->add_to_symbol_table(var, CHAR);

    vector<string> params{"!reg", to_string(retval)};
    (*listCFG->rbegin())->current_bb->add_IRInstr(ldconst, INT, params);

    vector<string> params2{var, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, CHAR, params2);

    return 0;
}

antlrcpp::Any IRVisitor::visitDeclalone(ifccParser::DeclaloneContext *ctx) {
    Type t;
    if ( (ctx->type->getText()).compare("int") == 0) {
        t = INT;

    } else {
        t = CHAR;
    }


    for(antlr4::tree::TerminalNode * i : ctx->VAR()) {
        string var = i->getText();
        if (var != "!reg") {
            (*listCFG->rbegin())->add_to_symbol_table(var, t);
        }
    }
    return 0;
}

// --------------------------------------- AFFFECTATION --------------------------------

antlrcpp::Any IRVisitor::visitExpression(ifccParser::ExpressionContext *ctx) {
    this->visit( ctx->expr() );

    return 0;
}

antlrcpp::Any IRVisitor::visitExpraff(ifccParser::ExpraffContext *ctx) {
    this->visit( ctx->expr() );

    string var = ctx->VAR()->getText();

    vector<string> params{var, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    return 0;
}