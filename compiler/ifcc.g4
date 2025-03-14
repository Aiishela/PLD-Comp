grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' stmt* return_stmt '}' ;

stmt : decl ';'         #declaration
        | aff ';'       #affectation
    ;

decl : 'int' VAR '=' CONST          #declconst
        | 'int' ( VAR ',')* VAR        #declalone
    ;

aff : VAR '=' CONST     #affconst
        | VAR '=' VAR   #affvar 
    ;

return_stmt: RETURN return_expr ';' ;

return_expr : CONST         #returnconst
        | VAR               #returnvar
    ;

RETURN : 'return' ;
CONST : [0-9]+ ;
VAR : [a-zA-Z_] [a-zA-Z_0-9]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
