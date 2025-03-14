grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' stmt* return_stmt '}' ;

stmt : decl ';'         #declaration
        | aff ';'       #affectation
    ;

decl : 'int' VAR '=' CONST          #declconst
        | 'int' ( VAR ',')* VAR        #declalone
    ;

aff : VAR '=' expr 
    ;

return_stmt: RETURN expr ';' ;

expr :  expr MULDIV expr       #exprmuldiv
        | expr ADDSUB expr  #expraddsub
        | CONST             #exprconst   
        | VAR               #exprvar
    ;

MULDIV : ('*'|'/') ;
ADDSUB : ('+'|'-') ;
RETURN : 'return' ;
CONST : [0-9]+ ;
VAR : [a-zA-Z_] [a-zA-Z_0-9]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
