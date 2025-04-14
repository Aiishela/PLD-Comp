#include "CFG.h"
#include "BasicBlock.h"
#include "IRInstr.h"

BasicBlock::BasicBlock(CFG* c, string entry_label){
    label = entry_label;
    cfg = c;
    exit_true = nullptr;
    exit_false = nullptr;
}

/** Renvoie la dernière instruction de instrs */
IRInstr *  BasicBlock::getLastInstr() {
    return instrs.back();
}

/** Pour l'optimisation : Supprime les instructions situées entre les deux index */
void BasicBlock::removeInstrs(int indexBegin, int indexEnd) {
    if (indexEnd == -1) {
        instrs.erase(next(instrs.begin(), indexBegin), instrs.end());
    } else {
        instrs.erase(next(instrs.begin(), indexBegin), next(instrs.begin(), indexEnd));
    }
}

/** Génération du code assembleur x86 pour le basic bloc
 * avec génération du jump conditionnel si nécessaire
 */
void BasicBlock::gen_asm(ostream &o){
    o << "." << label << ":\n";
    
    for (auto it = instrs.begin(); it != instrs.end(); ++it) {
        (*it)->gen_asm(o);
    }
    if(exit_true  && exit_false){
        o << "   cmpl  $0, " << cfg->get_var_index(test_var_name) << "(%rbp)\n";
        o << "   je ." << exit_false->label << "        # exit_false\n";
        o << "   jmp ." << exit_true->label << "        # exit_true\n";
    }
    else if(exit_true && !exit_false){
        o << "   jmp ." << exit_true->label << "        # only exit_true \n";
    }
    else{

    }
}

/** Ajout d'une IRInstr au Basic Block */
void BasicBlock::add_IRInstr(Operation op, Type t, vector<string> params){
    IRInstr * ins = new IRInstr(this, op, t, params);
    instrs.push_back(ins);
}

/** Optimization des instructions à la fin du parcours du programme
 * pour supprimer les lignes redondantes et réduire la longueur du code généré
 */
void BasicBlock::store_load_optim(){
    for (auto it = instrs.begin(); it != instrs.end(); ++it) {
        if ((*it)->getOp() == Operation::copy) {
            // Optimise movl %eax, -4(%rbp) (enlève la seconde ligne)
            //          movl -4(%rbp), %eax
            auto next_it = std::next(it);
            if (next_it != instrs.end() && (*next_it)->getOp() == Operation::copy) {
                vector<string> params1 = (*it)->getParams();
                vector<string> params2 = (*next_it)->getParams();
                
                if (params1[0] == params2[1] && params1[1] == params2[0]) {
                    instrs.erase(next_it); 
                }
            }
        }
        else if ((*it)->getOp() == ldconst) {
            // Optimise movl $0, %eax   ( plusieurs mêmes lignes à la suite)
            //          movl $0, %eax
            auto next_it = std::next(it);
            if (next_it != instrs.end() && (*next_it)->getOp() == ldconst) {
                vector<string> params1 = (*it)->getParams();
                vector<string> params2 = (*next_it)->getParams();
                
                if (params1[1] == params2[1]) {
                    instrs.erase(next_it);
                    --it;
                }
            }
        }
    }
    
}