#include "ASTGen.h"

void ASTGen::createNewCFG() 
{
    CFG * cfg = new CFG(this);
    listCFG->push_back(cfg); // current_bb créé
    
}



