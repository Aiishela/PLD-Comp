#include "IRVisitor.h"
#include <signal.h>

antlrcpp::Any IRVisitor::visitFunc(ifccParser::FuncContext *ctx) 
{
    CFG * cfg = new CFG(ctx->VAR()->getText());
    listCFG->push_back(cfg);
    //(*listCFG->rbegin())->add_to_symbol_table("!reg", INT); pas la peine parceque !reg = eax
    this->ret = false;
    this->visit( ctx->bloc() );
    /*for(ifccParser::StmtContext * i : ctx->stmt()) this->visit( i );
    this->visit( ctx->return_stmt() );*/
    
    return 0;
}

// ------------------------------------------ STRUCTURES DE CONTROLE -----------------------------------------

antlrcpp::Any IRVisitor::visitIfstmt(ifccParser::IfstmtContext *ctx) 
{
    this->visit( ctx->expr() );
    CFG * cfg=(*listCFG->rbegin());

    // Evaluation de l'expression booléenne, stockée dans test
    string test = cfg->create_new_tempvar(INT);
    vector<string> params{test, "!reg"};
    cfg->current_bb->add_IRInstr(Operation::copy, INT, params);

    // Testbb est le current bb
    BasicBlock* test_bb = cfg->current_bb ;
    test_bb->test_var_name = test;
    
    // Creation des blocs true, false et endif
    BasicBlock* then_bb = new BasicBlock(cfg,"trueCode" + test);
    cfg->add_bb(then_bb);

    BasicBlock* else_bb = new BasicBlock(cfg,"falseCode" + test);
    cfg->add_bb(else_bb);

    BasicBlock* endif_bb = new BasicBlock(cfg, "endif" + test);
    cfg->add_bb(endif_bb);

    // Ajout des stmt dans les différents blocs
    bool hasElse = ctx->bloc().size() == 2;

    cfg->current_bb = then_bb;
    this->visit( ctx->bloc()[0] );
    this->ret = false;
    if (hasElse) {
        cfg->current_bb = else_bb;
        this->visit( ctx->bloc()[1] );
        this->ret = false;
    } 

    // Lien entre les différents bb
    endif_bb->exit_true = test_bb->exit_true;
    endif_bb->exit_false = test_bb->exit_false;

    test_bb->exit_true = then_bb;
    test_bb->exit_false = else_bb;

    then_bb->exit_true = endif_bb;
    else_bb->exit_true = endif_bb;

    cfg->current_bb = endif_bb;

    return 0;
}

antlrcpp::Any IRVisitor::visitWhilestmt(ifccParser::WhilestmtContext *ctx) 
{
    CFG * cfg=(*listCFG->rbegin());
    BasicBlock* beforeWhileBB = cfg->current_bb;
    string test = cfg->create_new_tempvar(INT);

    // Creation des blocs body, afterWhile et test
    BasicBlock* test_bb = new BasicBlock(cfg,"testWhile" + test);
    cfg->add_bb(test_bb);
    test_bb->test_var_name = test;

    BasicBlock* body_bb = new BasicBlock(cfg,"bodyWhile" + test) ; 
    cfg->add_bb(body_bb);

    BasicBlock* afterWhile_bb = new BasicBlock(cfg,"afterWhile" + test) ; 
    cfg->add_bb(afterWhile_bb);

    // Passage dans le testBB avec ajout des instructions de l'expression
    cfg->current_bb = test_bb;
    this->visit( ctx->expr() );
    vector<string> params{test, "!reg"};
    cfg->current_bb->add_IRInstr(Operation::copy, INT, params);

    // Passage dans le bloc While et génération des instructions dans body_bb
    cfg->current_bb = body_bb;
    this->visit( ctx->bloc() ); // c'est possible qu'ici le current_bb change ( à cause d'un if par exemple)
    this->ret = false;

    // Lien entre les différents bb
    afterWhile_bb->exit_true = beforeWhileBB->exit_true;
    afterWhile_bb->exit_false = beforeWhileBB->exit_false;

    test_bb->exit_true = body_bb;
    test_bb->exit_false = afterWhile_bb;

    cfg->current_bb->exit_true = test_bb;

    cfg->current_bb = afterWhile_bb;
    
    return 0;
}

// -------------------------------------- EXPR -------------------------------------

antlrcpp::Any IRVisitor::visitExprconst(ifccParser::ExprconstContext *ctx) {
    int retval = stoi(ctx->CONST()->getText());

    vector<string> params{"!reg", to_string(retval)};
    (*listCFG->rbegin())->current_bb->add_IRInstr(ldconst, INT, params);
    return 0;
}

int getIntValue(const std::string& charConst) {
    if (charConst.size() < 3 || charConst.front() != '\'' || charConst.back() != '\'') {
        throw std::invalid_argument("Invalid character constant");
    }

    std::string inner = charConst.substr(1, charConst.size() - 2);

    // Handle escape sequences
    if (inner == "\\n") return '\n';
    if (inner == "\\t") return '\t';
    if (inner == "\\r") return '\r';
    if (inner == "\\0") return '\0';
    if (inner == "\\'") return '\'';
    if (inner == "\\\"") return '\"';
    if (inner == "\\\\") return '\\';

    // Multi-character constant case
    int result = 0;
    for (char ch : inner) {
        result = (result << 8) | static_cast<unsigned char>(ch);  // Shift left and add character
    }
    return result;
}

antlrcpp::Any IRVisitor::visitExprcharconst(ifccParser::ExprcharconstContext *ctx) {
    
    int length = ctx->CHARCONST()->getText().size();
    int retval = getIntValue(ctx->CHARCONST()->getText());

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

antlrcpp::Any IRVisitor::visitExprbracket(ifccParser::ExprbracketContext *ctx) {
    this->visit( ctx->expr() );
    return 0;
}

antlrcpp::Any IRVisitor::visitExprpostfix(ifccParser::ExprpostfixContext *ctx) {
    string var = ctx->VAR()->getText();

    // put the initial value into %eax and %rax (lower bits)
    vector<string> params{"!reg", var};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    // increment/decrement by 1 the value in var, without touching eax
    if ( (ctx->postfix->getText()).compare("++") == 0) {
        vector<string> params1{var};
        (*listCFG->rbegin())->current_bb->add_IRInstr(postIncr, INT, params1);
    } else {
        vector<string> params1{var};
        (*listCFG->rbegin())->current_bb->add_IRInstr(postDecr, INT, params1);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprprefix(ifccParser::ExprprefixContext *ctx) {
    string var = ctx->VAR()->getText();

    // increment/decrement by 1 the value in var
    if ( (ctx->prefix->getText()).compare("++") == 0) {
        vector<string> params1{var};
        (*listCFG->rbegin())->current_bb->add_IRInstr(preIncr, INT, params1);
    } else {
        vector<string> params1{var};
        (*listCFG->rbegin())->current_bb->add_IRInstr(preDecr, INT, params1);
    }

    // put the updated value into %eax
    vector<string> params2{"!reg", var};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params2);

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
    BasicBlock * current_bb = (*listCFG->rbegin())->current_bb;

    // Parcours de l'arbre de gauche : valeur dans %eax
    this->visit( ctx->expr()[0] );
    IRInstr * leftInstr = current_bb->getLastInstr();
    int leftPosition = current_bb->instrs.size();

    // Operation::copy de %eax dans tmp, lvalue dans tmp
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);
    vector<string> params{tmp, "!reg"};
    current_bb->add_IRInstr(Operation::copy, INT, params);

    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[1] );
    IRInstr * rightInstr = current_bb->getLastInstr();

    // Optimisation : Vérification de si les deux expressions sont des constantes 
    if (rightInstr->getOp() == ldconst && leftInstr->getOp() == ldconst) {
        int lvalue = leftInstr->getConstValue();
        int rvalue = rightInstr->getConstValue();
        

        // si les deux sont constants : mets directement le résultat du calcul        
        if ( (ctx->MULDIVMOD()->getText()).compare("*") == 0) {
            current_bb->removeInstrs(leftPosition);
            vector<string> paramsC{"!reg", to_string(lvalue * rvalue)};
            current_bb->add_IRInstr(ldconst, INT, paramsC);

        } else if ( (ctx->MULDIVMOD()->getText()).compare("/") == 0){ // TODO test division par 0
            vector<string> paramsSwap{tmp};
            current_bb->add_IRInstr(swap_, INT, paramsSwap);

            // opération
            vector<string> params2{"!reg", tmp};
            current_bb->add_IRInstr(div_, INT, params2);
            //vector<string> paramsC{"!reg", to_string(lvalue / rvalue)};
            //current_bb->add_IRInstr(ldconst, INT, paramsC);

        } else {
            current_bb->removeInstrs(leftPosition);
            vector<string> paramsC{"!reg", to_string(lvalue % rvalue)};
            current_bb->add_IRInstr(ldconst, INT, paramsC);
        }

    
    } else if (((ctx->MULDIVMOD()->getText()).compare("*") == 0 || (ctx->MULDIVMOD()->getText()).compare("/") == 0 )&& rightInstr->getOp() == ldconst && rightInstr->getConstValue() == 1){ 
        // multiplication ou division par 1 à droite
        current_bb->removeInstrs(leftPosition);
        
    } else if ((ctx->MULDIVMOD()->getText()).compare("*") == 0 && rightInstr->getOp() == ldconst && rightInstr->getConstValue() == 0){ 
        // multiplication par 0 à droite
        current_bb->removeInstrs(leftPosition);
        vector<string> paramsC{"!reg", "0"};
        current_bb->add_IRInstr(ldconst, INT, paramsC);
        
    } else if ((ctx->MULDIVMOD()->getText()).compare("*") == 0 && leftInstr->getOp() == ldconst && leftInstr->getConstValue() == 1){ 
        // multiplication par 1 à gauche
        current_bb->removeInstrs(leftPosition-1, leftPosition);

    } else if ((ctx->MULDIVMOD()->getText()).compare("*") == 0 && leftInstr->getOp() == ldconst && leftInstr->getConstValue() == 0){ 
        // multiplication par 0 à gauche
        current_bb->removeInstrs(leftPosition-1, leftPosition );
        vector<string> paramsC{"!reg", "0"};
        current_bb->add_IRInstr(ldconst, INT, paramsC);
        
    }else {
        // opération
        vector<string> params2{"!reg", tmp};

        if ( (ctx->MULDIVMOD()->getText()).compare("*") == 0) {
            current_bb->add_IRInstr(mul, INT, params2);

        } else if ( (ctx->MULDIVMOD()->getText()).compare("/") == 0){
            // Inversion de tmp et %eax pour opération dans le bon sens
            vector<string> paramsSwap{tmp};
            current_bb->add_IRInstr(swap_, INT, paramsSwap);
            current_bb->add_IRInstr(div_, INT, params2);

        } else {
            // Inversion de tmp et %eax pour opération dans le bon sens
            vector<string> paramsSwap{tmp};
            current_bb->add_IRInstr(swap_, INT, paramsSwap);
            current_bb->add_IRInstr(mod, INT, params2);
        }
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcomplg(ifccParser::ExprcomplgContext *ctx) {
    this->visit( ctx->expr()[0] );
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    this->visit( ctx->expr()[1] );

    // Inversion de tmp et %eax pour opération dans le bon sens
    vector<string> paramsSwap{tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(swap_, INT, paramsSwap);

    vector<string> params2{"!reg", tmp};

    if ( (ctx->COMPLG()->getText()).compare("<") == 0) {
        (*listCFG->rbegin())->current_bb->add_IRInstr(cmp_lt, INT, params2);
    } else { 
        (*listCFG->rbegin())->current_bb->add_IRInstr(cmp_gt, INT, params2);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprcompeqdiff(ifccParser::ExprcompeqdiffContext *ctx) {
    this->visit( ctx->expr()[0] );
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    this->visit( ctx->expr()[1] );

    // Inversion de tmp et %eax pour opération dans le bon sens
    vector<string> paramsSwap{tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(swap_, INT, paramsSwap);

    vector<string> params2{"!reg", tmp};

    if ( (ctx->COMPEQDIFF()->getText()).compare("==") == 0) {
        (*listCFG->rbegin())->current_bb->add_IRInstr(cmp_eq, INT, params2);
    } else { 
        (*listCFG->rbegin())->current_bb->add_IRInstr(cmp_neq, INT, params2);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExpraddsub(ifccParser::ExpraddsubContext *ctx) {
    BasicBlock * current_bb = (*listCFG->rbegin())->current_bb;

    // Parcours de l'arbre de gauche : valeur dans %eax
    this->visit( ctx->expr()[0] );
    IRInstr * leftInstr = current_bb->getLastInstr();
    int position = current_bb->instrs.size();
    
    // Operation::copy de %eax dans tmp
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);
    vector<string> params{tmp, "!reg"};
    current_bb->add_IRInstr(Operation::copy, INT, params);
    
    // Parcours de l'arbre de droite : valeur dans %eax
    this->visit( ctx->expr()[1] );
    IRInstr * rightInstr = current_bb->getLastInstr();
    
    // Optimisation : Vérification de si les deux expressions sont des constantes 
    if (rightInstr->getOp() == ldconst && leftInstr->getOp() == ldconst) {
        int lvalue = leftInstr->getConstValue();
        int rvalue = rightInstr->getConstValue();
        current_bb->removeInstrs(position);

        // si les deux sont constants : mets directement le résultat du calcul
        if ( (ctx->addsub->getText()).compare("+") == 0) {
            vector<string> paramsC{"!reg", to_string(lvalue + rvalue)};
            current_bb->add_IRInstr(ldconst, INT, paramsC);
    
        } else {
            vector<string> paramsC{"!reg", to_string(lvalue - rvalue)};
            current_bb->add_IRInstr(ldconst, INT, paramsC);
        }

    } else if (rightInstr->getOp() == ldconst && rightInstr->getConstValue() == 0){ 
        // addition ou soustraction par 0 à droite
        current_bb->removeInstrs(position);
        
    } else if ((ctx->addsub->getText()).compare("+") == 0 && leftInstr->getOp() == ldconst && leftInstr->getConstValue() == 0){ 
        // addition par 0 à gauche
        current_bb->removeInstrs(position-1, position);
        
    } else {
        
        // addition ou soustraction : %eax = %eax +- tmp
        vector<string> params2{"!reg", tmp};
        if ( (ctx->addsub->getText()).compare("+") == 0) {
            current_bb->add_IRInstr(add, INT, params2);
    
        } else {
            // Inversion de tmp et %eax pour opération dans le bon sens
            vector<string> paramsSwap{tmp};
            current_bb->add_IRInstr(swap_, INT, paramsSwap);    
            current_bb->add_IRInstr(sub, INT, params2);
        }
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprandbb(ifccParser::ExprandbbContext *ctx) {
    this->visit( ctx->expr()[0] );
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    this->visit( ctx->expr()[1] );

    // Inversion de tmp et %eax pour opération dans le bon sens
    vector<string> paramsSwap{tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(swap_, INT, paramsSwap);

    // Opération
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
    this->visit( ctx->expr()[0] );
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    this->visit( ctx->expr()[1] );

    // Inversion de tmp et %eax pour opération dans le bon sens
    vector<string> paramsSwap{tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(swap_, INT, paramsSwap);

    // Opération
    vector<string> params2{"!reg", tmp};
    (*listCFG->rbegin())->current_bb->add_IRInstr(orbb, INT, params2);

    return 0;
}
antlrcpp::Any IRVisitor::visitExprorbool(ifccParser::ExprorboolContext *ctx) {
    //Évaluation du premier opérande
    this->visit(ctx->expr()[0]);
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // Stocke le résultat du premier opérande
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    // Saut si  opérande1 !=  0 <=> opérande1 vrai
    string labelEnd = (*listCFG->rbegin())->new_BB_name();
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::jmp_if_true, INT, {tmp,labelEnd});

    // Évaluation du second opérande uniquement si nécessaire
    this->visit(ctx->expr()[1]);

    // Label de sortie si court-circuitage
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::label, INT, {labelEnd});;

    return 0;
}

antlrcpp::Any IRVisitor::visitExprandbool(ifccParser::ExprandboolContext *ctx) {
    this->visit(ctx->expr()[0]);
    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // Stocke le résultat du premier opérande
    vector<string> params{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    // Saut si  opérande1 ==  0 <=> opérande1 false
    string labelEnd = (*listCFG->rbegin())->new_BB_name();
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::jmp_if_false, INT, {tmp,labelEnd});

    // Évaluation du second opérande uniquement si nécessaire
    this->visit(ctx->expr()[1]);

    // Label de sortie si court-circuitage
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::label, INT, {labelEnd});;

    return 0;

}


// --------------------------------------- DECLARATION --------------------------------

antlrcpp::Any IRVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    this->visit( ctx->decl() );

    return 0;
}

antlrcpp::Any IRVisitor::visitDeclexpr(ifccParser::DeclexprContext *ctx) {
    Type t;
    if ( (ctx->type->getText()).compare("int") == 0) {
        t = INT;

    } else {
        t = CHAR;
    }

    this->visit( ctx->expr() );
    string var = ctx->VAR()->getText();

    (*listCFG->rbegin())->add_to_symbol_table(var, t);

    vector<string> params{var, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, t, params);

    return 0;
}

/*antlrcpp::Any IRVisitor::visitDeclchar(ifccParser::DeclcharContext *ctx) {
    string var = ctx->VAR()->getText();
    char c = ctx->CHAR->getText()[0];
    int retval = c;
    (*listCFG->rbegin())->add_to_symbol_table(var, CHAR);

    vector<string> params{"!reg", to_string(retval)};
    (*listCFG->rbegin())->current_bb->add_IRInstr(ldconst, INT, params);

    vector<string> params2{var, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, CHAR, params2);

    return 0;
}*/

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
    string symbol = ctx->affsymbol->getText();

    string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);

    // met la valeur de %eax dans tmp
    vector<string> params0{tmp, "!reg"};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params0);
    // met la veleur de var dans %eax
    vector<string> params00{"!reg", var};
    (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params00);
    // on a lvalue dans tmp et rvalue dans %eax


    if (symbol.compare("=") == 0) {
        vector<string> params{var, tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

    } else if (symbol.compare("+=") == 0) {
        vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(add, INT, params);
        vector<string> params2{var, "!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params2)
        ;
    } else if (symbol.compare("-=") == 0) {
        vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(sub, INT, params);
        vector<string> params2{var, "!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params2);

    } else if (symbol.compare("*=") == 0) {
        vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(mul, INT, params);
        vector<string> params2{var, "!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params2);

    } else if (symbol.compare("/=") == 0) {
        vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(div_, INT, params);
        vector<string> params2{var, "!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params2);

    } else if (symbol.compare("%=") == 0) {
        vector<string> params{"!reg", tmp};
        (*listCFG->rbegin())->current_bb->add_IRInstr(mod, INT, params);
        vector<string> params2{var, "!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params2);
    }


    return 0;
}


// --------------------------------------- BLOC --------------------------------
antlrcpp::Any IRVisitor::visitBloc(ifccParser::BlocContext *ctx) {
    for(ifccParser::StmtContext * i : ctx->stmt()){
        this->visit( i );
        if(this->ret == true){
            vector<string> params{"epilogue"};
            (*listCFG->rbegin())->current_bb->add_IRInstr(jmp, INT, params);  
            break;
        }
    }
    return 0;
}

// --------------------------------------- APPEL FONCTION --------------------------------

antlrcpp::Any IRVisitor::visitCallfunc(ifccParser::CallfuncContext *ctx) {
    string func_name = ctx->VAR()->getText();  // function name is the first token

    if (func_name == "putchar" || func_name == "getchar" ) {
        func_name += "@PLT";
    }

    vector<string> call_instr_params = {func_name};

    // Visit each expr and store its value in a temporary variable
    for (auto expr_ctx : ctx->expr()) {
        // Evaluate expression, result goes into %eax
        this->visit(expr_ctx);

        // Create a temp var and copy %eax into it
        string tmp = (*listCFG->rbegin())->create_new_tempvar(INT);
        vector<string> params{tmp, "!reg"};
        (*listCFG->rbegin())->current_bb->add_IRInstr(Operation::copy, INT, params);

        // Add this temp to the parameter list
        call_instr_params.push_back(tmp);
    }

    (*listCFG->rbegin())->current_bb->add_IRInstr(call, INT, call_instr_params);

    return 0;
}

// --------------------------------------- RETURN --------------------------------

antlrcpp::Any IRVisitor::visitReturn(ifccParser::ReturnContext *ctx) {

    this->visit( ctx->expr());
    this->ret = true;

    return 0;
}
