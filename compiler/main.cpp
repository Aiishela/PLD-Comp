#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"


#include "IR.h"
#include "IRVisitor.h"

using namespace antlr4;
using namespace std;
extern SymbolTable *symbolTable;
extern CFG *cfg;

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
    cfg = new CFG();
    symbolTable = new SymbolTable();

    streambuf *coutBuf = cout.rdbuf();
    cout.rdbuf(outFile.rdbuf());

    cout.rdbuf(coutBuf);
    outFile.close();
    
    IRVisitor irV;
    irV.visit(tree);

    return 0;
}
