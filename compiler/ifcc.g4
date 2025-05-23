grammar ifcc;

axiom : func* EOF ;

func : type=('int'|'void') VAR '(' ( types VAR ',')* (types VAR)? ')' bloc ;

types : 'int' | 'char' ;

stmt : decl ';'         #declaration
        | expr ';'      #expression
        | RETURN expr? ';' #return
        | 'if' '(' expr ')' bloc ('else' bloc)?       #ifstmt
        | 'while' '(' expr ')' bloc                   #whilestmt
        | bloc                                        #blocstmt
    ;

decl : type=('int'|'char') VAR '=' expr                #declexpr
        | type=('int'|'char') ( VAR ',')* VAR        #declalone
        | 'int' VAR '[' CONST? ']' '=' '{' ( expr ',')* expr  '}'  #decltab
        | 'int' VAR '[' CONST ']'                   #decltabempty
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
        | VAR ('[' expr ']')? affsymbol=('='|'+='|'-='|'*='|'/='|'%=') expr              #expraff
        | expr '&&' expr                        #exprandbool
        | expr '||' expr                        #exprorbool
        | VAR affsymbol=('='|'+='|'-='|'*='|'/='|'%=') expr              #expraff
        | CONST                                 #exprconst   
        | VAR                                   #exprvar
        | CHARCONST                             #exprcharconst
        | VAR '[' expr ']'                     #exprtab
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