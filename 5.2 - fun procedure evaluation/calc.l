/** calculator calc.l
 * origin source by Tom Niemann at epaperpress.com/lexandyacc
 * revision Lorenzo Massimo Gramola (2014)
 */

%{
    #include <stdlib.h>
    #include <strings.h>
    #include "calc.h" 
    #include "y.tab.h"


    void yyerror(char *);
%}

%%

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;
"for"           return FOR;
"to"            return TO;
int             return INT;
float           return FLOAT;
main            return MAIN;
[a-zA-Z][a-zA-Z_]*   {
                        symrec * s;
                        char * variableName = malloc(strlen(yytext));
                        strcpy(variableName,yytext);
                        s = getsym(variableName);
                        if (s == 0){
                          s =(symrec *) malloc (sizeof (symrec));
                          s->name = (char *) malloc(strlen(yytext));
                          strcpy (s->name,yytext);
                         }
                         *((symrec**)&yylval) = s;
                         return VARIABLE;
                    }
0               {
                    yylval.iValue = atoi(yytext);
                    return INTEGER;
                }
[1-9][0-9]*     {
                    yylval.iValue = atoi(yytext);
                    return INTEGER;
                }

[-+<>=*/.]   { return *yytext; }




[ \t\n]         ;/* skip whitespace */
\;              return SEMICOLON;
\,              return COMMA;
\(              return LP;
\)              return RP;
\{              return LCURLY;
\}              return RCURLY;
.               {ECHO;
                yyerror("Previous char is unknown to lexer");}

%%

int yywrap(void) {
    return 1;
}