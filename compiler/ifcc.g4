grammar ifcc;

axiom : func EOF ;


func : 'int' VAR '(' ')' bloc ;

stmt : decl ';'         #declaration
        | expr ';'      #expression
        | RETURN expr ';' #return
        | 'if' '(' expr ')' bloc ('else' bloc)?       #ifstmt
        | 'while' '(' expr ')' bloc                   #whilestmt
    ;

decl : type=('int'|'char') VAR '=' expr                #declexpr
        | type=('int'|'char') ( VAR ',')* VAR        #declalone
    ;

bloc : '{'  stmt*  '}'     
    ;

expr :  '(' expr ')'                            #exprbracket
        | VAR postfix=('++'|'--')               #exprpostfix
        | prefix=('++'|'--') VAR                #exprprefix
        | unaire=('!'|'-') expr                 #exprunaire
        | expr MULDIVMOD expr                   #exprmuldivmod
        | expr addsub=('+'|'-') expr            #expraddsub
        | expr COMPLG expr                      #exprcomplg
        | expr COMPEQDIFF expr                  #exprcompeqdiff
        | expr '&' expr                         #exprandbb
        | '~' expr                              #exprnotbb
        | expr '|' expr                         #exprorbb
        | VAR affsymbol=('='|'+='|'-='|'*='|'/='|'%=') expr              #expraff
        | CONST                                 #exprconst   
        | VAR                                   #exprvar
        | CHARCONST                             #exprcharconst
        | VAR '[' CONST ']'                     #exprtab
        | VAR '(' ( expr ',')* expr? ')'  #callfunc
    ;

CHARCONST : '\'' ( '\\' [ntr0\\'] | ~['\\] )+ '\'';
MULDIVMOD : ('*'|'/'|'%') ;
COMPLG : ('<'|'>') ;
COMPEQDIFF : ('=='|'!=') ;
RETURN : 'return' ;
CONST : [0-9]+ ;
VAR : [a-zA-Z_] [a-zA-Z_0-9]* ;
COMMENT : ('/*' .*? '*/'|'//' .*? '\n') -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);