#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "CodeGenVisitor.h"
#include "VariableCheckVisitor.h"
#include "SymbolTable.h"

using namespace antlr4;
using namespace std;
extern SymbolTable *symbolTable;

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

    symbolTable = new SymbolTable();

    streambuf *coutBuf = cout.rdbuf();
    cout.rdbuf(outFile.rdbuf());

    VariableCheckVisitor vc;
    vc.visit(tree);

    symbolTable->checkUsageST();

    cout.rdbuf(coutBuf);
    outFile.close();
    
    CodeGenVisitor v;
    v.visit(tree);

    return 0;
}
