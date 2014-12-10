/** calculator calc.y
 * origin source by Tom Niemann at epaperpress.com/lexandyacc
 * revision Lorenzo Massimo Gramola (2014)
*/

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include "calc.h"
    

    int ex(nodeType *p);
    int yylex(void);
    
    void yyerror(char *s);
    int sym[26];                    /* symbol table */
    int flagSkip = 0;
    int insideLoop = 0;
    %}
%error-verbose

%union {
    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */
};                              /* this cause a new type def to be generated,
                                 which is a union of the above and and is called
                                 YYSTYPE. Then there is the declaration
                                 extern YYSTYPE yylval which declares yylval as 
                                 an external variable
                                 */

%token <iValue> INTEGER         /* NOTE THIS DELCARATION*/
%token <sIndex> VARIABLE
%token WHILE IF PRINT FOR TO SKIP
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list

%%

program:
        function                   {exit(0);}
        ;

function:
        function stmt              {ex($2);freeNode($2);}
        | /* NULL */
        ;

stmt:
        ';'                                 {$$ = opr(';', 2, NULL, NULL);} //opr is of type "nodetype", we are putting in the stack a node
        | expr ';'                           {$$ = $1;}
        | PRINT expr ';'                     {$$ = opr(PRINT,1,$2);}
        | VARIABLE '=' expr ';'              {$$ = opr('=',2,id($1),$3);} // remember id is another funcion that returns a nodetype, we are passing a nodetype as id
        | WHILE '(' expr ')' stmt            {$$ = opr(WHILE,2,$3,$5);}
        | IF '(' expr ')' stmt %prec IFX     {$$ = opr(IF,2,$3,$5);}
        | IF '(' expr ')' stmt ELSE stmt     {$$ = opr(IF,3,$3,$5,$7);}
        | FOR '(' VARIABLE '=' expr TO expr ')' stmt    {$$ = opr(FOR,4,id($3),$5,$7,$9);}
        | '{' stmt_list '}'                  {$$ = $2;}
        | SKIP ';'                           {$$ = opr(SKIP,2,NULL,NULL);}
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
        | '(' expr ')'          {$$ = $2;}
        ;

%%


/**
 we defined the funtcion in the fist section of this file,
 now is time to provide a structure and some instruction to those funcitons
 
 nodeType *opr(int oper, int nops, ...);
 nodeType *id(int i);
 nodeType *con(int value);
 void freeNode(nodeType *p);
 int ex(nodeType *p);
 int yylex(void);
 void yyerror(char *s);
 */

nodeType *con(int value){
    nodeType *p;
    /* allocate node space in memory */
    if((p=malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    /* copy information */
    p->type = typeCon;
    p->con.value = value;
    
    return p;
}

nodeType *id (int i){
    nodeType *p;
    if((p=malloc(sizeof(nodeType)))==NULL){
        yyerror("out of memory");
    }
    p->type = typeId;
    p->id.i=i;
    
    return p;
}
nodeType *opr(int oper, int nops, ...){
    va_list ap; /* (ap = argument pointer) va_list is used to declare a variable
                 which, from time to time, is referring to an argument*/
    nodeType *p;
    int i;
    
    if ((p = malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    if((p->opr.op = malloc(nops*sizeof(nodeType)))== NULL){
        yyerror("out of memory");
    }
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);/* initialize the sequence, makes ap point to the first
                        anonymous argument. Must call it once before reading all
                        the parameters*/
    for(i = 0; i<nops;i++){
        p->opr.op[i]=va_arg(ap,nodeType*); /*every time va_arg is called it returns
                                            an argument and moves the pointer forward
                                            to the next argument. It uses a type name
                                            to decide what kind of type to return and
                                            how much to move forward the pointer.
                                            */
    }
    va_end(ap); /* MUST be called BEFORE THE FUNCTION TERMINATES. it provides
                 the necessary cleaning functions.*/


    return p;
}

void freeNode(nodeType *p){
    int i;
    if(!p) return;
    if(p->type == typeOpr){
        for(i=0;i<p->opr.nops; i++){
            freeNode(p->opr.op[i]);
        }
        free(p->opr.op);
    }
    free(p);
}

void yyerror(char *s){
    fprintf(stdout,"%s\n",s);
}
int main(){
    yyparse();
    return 0;
}