#include "VariableCheckVisitor.h"
#include "IRInstr.h"
#include "BasicBlock.h"
extern SymbolTable * symbolTable;

antlrcpp::Any VariableCheckVisitor::visitFunc(ifccParser::FuncContext *ctx) 
{
    
    std::string func_name = ctx->VAR(0)->getText();

    //Check if function name already exists
    if (ft->find(func_name) != ft->end()) {
        int line = ctx->getStart()->getLine();
        int col = ctx->getStart()->getCharPositionInLine();
        std::cerr << "[ERROR] Line " << line << ":" << col
                  << " → function '" << func_name << "' already defined.\n";
        exit(1);
    }
    
    // Create CFG with function name
    CFG * cfg = new CFG(func_name);
    listCFG->push_back(cfg);

    // Number of parameters = total VARs - 1 (excluding function name)
    int nb_params = ctx->VAR().size() - 1;
    std::vector<Type> type_list;

    if (nb_params > 0) {
        for (int i = 0; i < nb_params && i < 6; ++i) {
            std::string param_name = ctx->VAR()[i + 1]->getText();
            std::string type_str = ctx->types(i)->getText(); 
            Type t = (type_str == "char") ? CHAR : INT;

            type_list.push_back(t);
    
            //on ajoute le parametre à la symbole table 
            int line = ctx->getStart()->getLine();
            int col = ctx->getStart()->getCharPositionInLine();
            (*listCFG->rbegin())->symbolTable->addVariable(param_name,t, line, col);
            (*listCFG->rbegin())->symbolTable->defineVariable(param_name, line, col);
    
        }
    }
    
    FuncInfo funcInfo = {nb_params, type_list};
    (*ft)[func_name] = funcInfo;

    this->visit(ctx->bloc());

    return 0;
}

// ------------------------------------------ STRUCTURES DE CONTROLE -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitIfstmt(ifccParser::IfstmtContext *ctx)
{
    this->visit( ctx->expr() );
    bool hasElse = ctx->bloc().size() == 2;

    CFG * cfg = (*listCFG->rbegin());
    SymbolTable st_original = *cfg->symbolTable; // deep copy

    if (hasElse) {
        // si il y a un else : passage dans le then ou le else, il faut donc vérifier mettre dans la table uniquement
        // les changements fait dans les deux blocs (intersection des changements)
        this->visit(ctx->bloc()[0]);
        SymbolTable st_then = *cfg->symbolTable;
    
        // passage dans le else avec la st originale
        *cfg->symbolTable = st_original;
        this->visit(ctx->bloc()[1]);
        SymbolTable st_else = *cfg->symbolTable;
    
        // Met l'intersection des deux dans st_original
        // i.e. il faut que les booléens defined, declared et used soit true dans les deux
        // pour que ça soit true dans st_original
        for (auto it = st_original.st->begin(); it != st_original.st->end();) {
            const std::string &varName = it->first;
    
            auto thenVar = st_then.st->find(varName);
            auto elseVar = st_else.st->find(varName);
    
            if (thenVar != st_then.st->end() && elseVar != st_else.st->end()) {
                cout << varName << endl;
                it->second.declared = thenVar->second.declared && elseVar->second.declared;
                it->second.defined = thenVar->second.defined && elseVar->second.defined;
                it->second.used = thenVar->second.used && elseVar->second.used;
    
                ++it;
            } else {
                it = st_original.st->erase(it);
            }
        }
        
        *cfg->symbolTable = st_original;
    } else {
        // présence du then uniquement : il faut avoir la même table de symbole qu'avant le passage dans le then
        // car si on est pas allé dans le then : la table n'a pas changé
        this->visit(ctx->bloc()[0]);
        *cfg->symbolTable = st_original;
    }

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitWhilestmt(ifccParser::WhilestmtContext *ctx) 
{
    CFG * cfg = (*listCFG->rbegin());
    
    this->visit( ctx->expr() );
    SymbolTable st_original = *cfg->symbolTable; // deep copy
    
    this->visit( ctx->bloc() ); 

    *cfg->symbolTable = st_original; // retour à la st originale car on peut passer 0 fois dans le while

    
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitBlocstmt(ifccParser::BlocstmtContext *ctx) {
    this->visit( ctx->bloc() );
    return 0;
}

// ------------------------------------------ EXPR -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitExprconst(ifccParser::ExprconstContext *ctx) {
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprcharconst(ifccParser::ExprcharconstContext *ctx) {
    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitExprtab(ifccParser::ExprtabContext *ctx) {
    this->visit( ctx->expr() );
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();

    string tab = ctx->VAR()->getText();
    (*listCFG->rbegin())->symbolTable->useVariable(tab , line, col);
    return 0;

}

antlrcpp::Any VariableCheckVisitor::visitExprvar(ifccParser::ExprvarContext *ctx) {
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();
    std::string var = ctx->VAR()->getText();
    (*listCFG->rbegin())->symbolTable->useVariable(var, line, col);

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

antlrcpp::Any VariableCheckVisitor::visitExprorbool(ifccParser::ExprorboolContext *ctx) {
    std::any val1 = this->visit(ctx->expr()[0]);

    // Vérifie si val1 contient une valeur et essaye de le caster en int
    int intVal1 = (val1.has_value() && val1.type() == typeid(int)) ? std::any_cast<int>(val1) : 0;

    if(intVal1 == 0) {
        this->visit( ctx->expr()[1] );
    }

    return 0;
}

// ------------------------------------------ DECLARATION -------------------------------------

antlrcpp::Any VariableCheckVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    this->visit( ctx->decl() );

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitDeclexpr(ifccParser::DeclexprContext *ctx) { 
    Type t;
    if ( (ctx->type->getText()).compare("int") == 0) {
        t = INT;

    } else {
        t = CHAR;
    }

    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();

    this->visit( ctx->expr() );
    std::string var = ctx->VAR()->getText();

    (*listCFG->rbegin())->symbolTable->addVariable(var, t, line, col);
    (*listCFG->rbegin())->symbolTable->defineVariable(var, line, col);

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitDecltab(ifccParser::DecltabContext *ctx) {
    string var = ctx->VAR()->getText();  
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();

    // Taille du tableau
    int sizeTab = -1;
    if (ctx->CONST() != nullptr) {
        sizeTab = stoi(ctx->CONST()->getText());
        int nbExpr = ctx->expr().size();
        if (sizeTab != nbExpr) {
            (*listCFG->rbegin())->symbolTable->printWarning("Wrong number of elements in array. " + to_string(nbExpr) + " != " + to_string(sizeTab), line, col);
        }
    } else {
        sizeTab = ctx->expr().size();
    }


    (*listCFG->rbegin())->symbolTable->addVariable(var, INT, line, col);
    (*listCFG->rbegin())->symbolTable->defineVariable(var, line, col);

    for (int index =0; index<sizeTab; index++) {
        this->visit(ctx->expr()[index]);

        (*listCFG->rbegin())->symbolTable->addVariable(var + "-" + to_string(index), INT, line, col);
        (*listCFG->rbegin())->symbolTable->defineVariable(var + "-" + to_string(index), line, col);

    }

    return 0;
}

antlrcpp::Any VariableCheckVisitor::visitDecltabempty(ifccParser::DecltabemptyContext *ctx) {
    string var = ctx->VAR()->getText();  

    // Taille du tableau
    int sizeTab = stoi(ctx->CONST()->getText());

    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();

    (*listCFG->rbegin())->symbolTable->addVariable(var, INT, line, col);
    (*listCFG->rbegin())->symbolTable->defineVariable(var, line, col);

    // Mettre des 0 dans toutes les cases
    for (int index =0; index<sizeTab; index++) {

        (*listCFG->rbegin())->symbolTable->addVariable(var + "-" + to_string(index), INT, line, col);
        (*listCFG->rbegin())->symbolTable->defineVariable(var + "-" + to_string(index), line, col);

    }

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

    if (ctx->expr().size() == 1) {
        this->visit( ctx->expr()[0] );
    } else {
        this->visit( ctx->expr()[0] );

        this->visit( ctx->expr()[1] );
    }

    std::string var = ctx->VAR()->getText();

    (*listCFG->rbegin())->symbolTable->defineVariable(var, line, col);
    // += est ce utilisé?
 
    return 0;
}


// ------------------------------------------ BLOC -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitBloc(ifccParser::BlocContext *ctx)
{
    for(ifccParser::StmtContext * i : ctx->stmt()){
        this->visit( i );
    }

    return 0;
}


// ------------------------------------------ RETURN -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitReturn(ifccParser::ReturnContext *ctx)
{
    this->visit( ctx->expr() );

    return 0;
}

// ------------------------------------------ APPEL FONCTON -----------------------------------------

antlrcpp::Any VariableCheckVisitor::visitCallfunc(ifccParser::CallfuncContext *ctx)
{
    string func_name = ctx->VAR()->getText();  // function name is the first token

    if (func_name == "putchar" || func_name == "getchar" ) {
        func_name += "@PLT";
        for (auto expr_ctx : ctx->expr()) this->visit(expr_ctx);
        return 0;
    }

    int arg_count = ctx->expr().size();
    int line = ctx->getStart()->getLine();
    int col = ctx->getStart()->getCharPositionInLine();

    pendingCalls.push_back({func_name, arg_count, line, col});
    // Visit expressions (to detect variable usage, etc.)
    for (auto expr_ctx : ctx->expr()) {
        this->visit(expr_ctx);
    }


    return 0;
}