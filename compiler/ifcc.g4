grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' stmt* return_stmt '}' ;

stmt : decl ';'         #declaration
        | aff ';'       #affectation
    ;

decl : 'int' VAR '=' expr              #declexpr
        | 'int' ( VAR ',')* VAR        #declalone
    ;

aff : VAR '=' expr 
    ;

return_stmt: RETURN expr ';' ;

expr :  '(' expr ')'                #exprbracket
        | unaire=('!'|'-') expr               #exprunaire
        | expr MULDIVMOD expr       #exprmuldivmod
        | expr addsub=('+'|'-') expr          #expraddsub
        | expr COMPLG expr          #exprcomplg
        | expr COMPEQDIFF expr      #exprcompeqdiff
        | expr '&' expr             #exprandbb
        | '~' expr                  #exprnotbb
        | expr '|' expr             #exprorbb
        | CONST                     #exprconst   
        | VAR                       #exprvar
    ;

MULDIVMOD : ('*'|'/'|'%') ;
COMPLG : ('<'|'>') ;
COMPEQDIFF : ('=='|'!=') ;
RETURN : 'return' ;
CONST : [0-9]+ ;
VAR : [a-zA-Z_] [a-zA-Z_0-9]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
