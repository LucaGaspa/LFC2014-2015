/** calculator calc.y
 * origin source by Tom Niemann at epaperpress.com/lexandyacc
 * revision Lorenzo Massimo Gramola (2014)
*/


/**
mackbook-pro:5.0 - calc functions Lorenzo$ yacc calc.y 
calc.y:46.14: warning: empty rule for typed nonterminal, and no action
2 rules never reduced
calc.y: warning: 1 useless nonterminal and 2 useless rules
calc.y:73.1-8: warning: useless nonterminal: function
calc.y:74.9-21: warning: useless rule: function: function stmt
calc.y:75.10: warning: useless rule: function: empty 

*/
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <strings.h>
    #include "calc.h"

    int yylex(void);    
    void yyerror(char *s);
    symrec * symTable;
    /**
        temporary variable declarations
    */
    linkedList * temp_list = NULL;
    linkedList * programRoutineList = NULL;
    linkedList * argList = NULL;
    program * prg;

%}
%error-verbose

%union {
    int iValue;                 
    symrec * sRec;
    nodeType *nPtr;

    basicType basic;  
    parameter *parameter;
    routine * routine;
    linkedList *lista;       
};                          

%token <iValue> INTEGER         
%token <sRec> VARIABLE
%token WHILE IF PRINT FOR TO
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS RCURLY LCURLY LP RP COMMA SEMICOLON  INT FLOAT  MAIN

%type <basic> B
%type <parameter> param
%type <routine> proc
%type <lista> param_list proc_list opt_param_list opt_proc_list arg_list opt_arg_list
%type <nPtr> stmt expr stmt_list program opt_stmt_list arg

%%

program: opt_proc_list                              
        MAIN  
        opt_stmt_list                               {
                                                        prg = (program*)malloc(sizeof(program));
                                                        prg->routineList = $1;
                                                        prg->commandList = $3;
                                                        prg->symtable = symTable;
                                                        executeProgram(prg, prg->symtable);   
                                                        exit(0);
                                                    }
        ;
opt_stmt_list: 
             /*empty*/ 
            | stmt_list                                  { $$ = $1;}
                ;
opt_proc_list:
        /*empty*/
        | proc_list                                 { $$ = $1; }
        ;
proc_list:
        proc                                        {
                                                        if(programRoutineList!=NULL){
                                                            programRoutineList = NULL;
                                                        }
                                                        $$ = insertRoutine(&programRoutineList, $1);
                                                    }
        | proc_list SEMICOLON proc                  {
                                                        $$ = insertRoutine(&$1, $3);
                                                    }      
        ;
proc:    
        VARIABLE LP opt_param_list RP LCURLY 
                opt_stmt_list 
            RCURLY                                  {$$ = createRoutine($1->name, PROCEDURE, NULL,$3,$6);}
        
        |B VARIABLE LP opt_param_list RP LCURLY 
                opt_stmt_list                       
            RCURLY                                  {$$ = createRoutine($2->name, FUNCTION, &$1,$4,$7);}
        ;
B:      INT                                         { $$ = INTTYPE; }
        | FLOAT                                     { $$ = FLOATTYPE;}
        ;
opt_param_list:
         /*empty*/
         |param_list                                    { 
                                                            $$ = invertParamOrder($1);}
         ;
param_list:
        param                                           {   
                                                            if(temp_list!=NULL){
                                                                //clean list
                                                                temp_list = NULL;
                                                            }
                                                            $$ = insertParam(&temp_list, $1);
                                                        }
        | param_list COMMA param                        {$$ = insertParam(&$1, $3);}
        ;
param:  B VARIABLE                                      { $$ = createParam($2->name,$1);}
        ;
opt_arg_list:
        /*empty*/
        |arg_list                                       { $$ = invertArgOrder($1);}
        ;
arg_list:
        arg                                             {   
                                                            if(argList!=NULL){
                                                                //clean list
                                                                argList = NULL;
                                                            }
                                                            $$ = insertArg(&argList, $1);
                                                        }    
        |arg_list COMMA arg                             {$$ = insertArg(&$1, $3);}
        ;
arg:    expr                                         { $$ = $1;}
        ;  
stmt:
        SEMICOLON                                       {$$ = opr(';', 2, NULL, NULL);} 
        | expr SEMICOLON                                {$$ = $1;}
        | PRINT expr SEMICOLON                          {$$ = opr(PRINT,1,$2);}
        | VARIABLE '=' expr SEMICOLON                   {$$ = opr('=',2,id($1),$3);}     
        | WHILE LP expr RP stmt                         {$$ = opr(WHILE,2,$3,$5);}
        | IF LP expr RP stmt %prec IFX                  {$$ = opr(IF,2,$3,$5);}
        | IF LP expr RP stmt ELSE stmt                  {$$ = opr(IF,3,$3,$5,$7);}
        | FOR LP VARIABLE '=' expr TO expr RP stmt      {$$ = opr(FOR,4,id($3),$5,$7,$9);}
        | LCURLY stmt_list RCURLY                       {$$ = $2;}
        ;


stmt_list:
        stmt                    { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;
expr:
        INTEGER                 {$$ = con($1);} //manage constants
        | VARIABLE              {$$ = id($1);} //manage variables - namely an IDENTIFIER
        | '-' expr %prec UMINUS {$$ = opr(UMINUS,1,$2);}
        | expr '+' expr         {$$ = opr('+',2,$1,$3);}
        | expr '-' expr         {$$ = opr('-',2,$1,$3);}
        | expr '*' expr         {$$ = opr('*',2,$1,$3);}
        | expr '/' expr         {$$ = opr('/',2,$1,$3);}
        | expr '<' expr         {$$ = opr('<',2,$1,$3);}
        | expr '>' expr         {$$ = opr('>',2,$1,$3);}
        | expr GE expr          {$$ = opr(GE,2,$1,$3);}
        | expr LE expr          {$$ = opr(LE,2,$1,$3);}
        | expr NE expr          {$$ = opr(NE,2,$1,$3);}
        | expr EQ expr          {$$ = opr(EQ,2,$1,$3);}
        | LP expr RP            {$$ = $2;}
        | VARIABLE LP 
            opt_arg_list RP     {  
                                    $$ = rout($1->name,$3);
                                }
        ;

%%


void yyerror(char *s){
    fprintf(stdout,"%s\n",s);
}
int main(){
    yyparse();
    return 0;
}