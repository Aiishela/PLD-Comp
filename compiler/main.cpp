#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"


#include "CFG.h"
#include "IRInstr.h"
#include "BasicBlock.h"
#include "IRVisitor.h"
#include "VariableCheckVisitor.h"
#include "SymbolTable.h"

using namespace antlr4;
using namespace std;

int main(int argn, const char **argv)
{
    stringstream in;
    if (argn == 2)
    {
        ifstream lecture(argv[1]);
        if (!lecture.good())
        {
            cerr << "error: cannot read file: " << argv[1] << endl;
            exit(1);
        }
        in << lecture.rdbuf();
    }
    else
    {
        cerr << "usage: ifcc path/to/file.c" << endl;
        exit(1);
    }

    ANTLRInputStream input(in.str());

    ifccLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();

    ifccParser parser(&tokens);
    tree::ParseTree *tree = parser.axiom();

    if (parser.getNumberOfSyntaxErrors() != 0)
    {
        cerr << "error: syntax error during parsing" << endl;
        exit(1);
    }
    ofstream outFile("erreur.s");
    if (!outFile)
    {
        cerr << "Error: Could not open output file." << endl;
        exit(1);
    }

    streambuf *coutBuf = cout.rdbuf();
    cout.rdbuf(outFile.rdbuf());

    VariableCheckVisitor vc;
    vc.visit(tree);

    cout.rdbuf(coutBuf);
    outFile.close();

    bool error = false;

    for (auto it = vc.listCFG->begin(); it != vc.listCFG->end(); ++it) {
        (*it)->symbolTable->checkUsageST();
        if ((*it)->symbolTable->getError() == true) {
            error = true;
        }
    }


    if (error) {
        ifstream f("erreur.s");
        //cout << "Error while parsing the file." << endl;
        if (f.is_open())
            cout << f.rdbuf(); 
        return 1;
    } else {
        
        IRVisitor irV;
        irV.visit(tree);

        ofstream outFile("asm-ifcc.s");
        if (!outFile)
        {
            cerr << "Error: Could not open output file." << endl;
            exit(1);
        }
        streambuf *coutBuf = cout.rdbuf();
        cout.rdbuf(outFile.rdbuf());
        for (auto cfg : *(irV.listCFG)) {
            cfg->store_load_optim();   // optional: apply optimizations
            cfg->gen_asm(cout);        // generate assembly
        }        

        cout.rdbuf(coutBuf);
        outFile.close();
    }


    return 0;
}
