grammar ifcc;

axiom : func EOF ;


func : 'int' VAR '(' ')' bloc ;

stmt : decl ';'         #declaration
        | expr ';'      #expression
    ;

decl : 'int' VAR '=' expr                            #declexpr
        | 'char' VAR '=' '\'' CHAR=. '\''              #declchar
        | type=('int'|'char') ( VAR ',')* VAR        #declalone
    ;

bloc : '{'  stmt* return_stmt '}'     
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
        | VAR '=' expr              #expraff
        | CONST                     #exprconst   
        | VAR                       #exprvar
        | '\'' CHAR=. '\''          #exprchar
        | VAR '(' ( expr ',')* expr? ')'  #callfunc
    ;

MULDIVMOD : ('*'|'/'|'%') ;
COMPLG : ('<'|'>') ;
COMPEQDIFF : ('=='|'!=') ;
RETURN : 'return' ;
CONST : [0-9]+ ;
VAR : [a-zA-Z_] [a-zA-Z_0-9]* ;
COMMENT : ('/*' .*? '*/'|'//' .*? '\n') -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);