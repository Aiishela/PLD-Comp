grammar ifcc;

axiom : func EOF ;

func : 'int' VAR '(' ')' '{' stmt* '}' ;

stmt : decl ';'         #declaration
        | expr ';'      #expression
        | RETURN expr ';' #return

    ;

decl : 'int' VAR '=' expr                            #declexpr
        | 'char' VAR '=' '\'' CHAR=. '\''              #declchar
        | type=('int'|'char') ( VAR ',')* VAR        #declalone
    ;

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