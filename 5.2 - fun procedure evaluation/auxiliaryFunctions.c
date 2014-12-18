#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <strings.h>
#include "calc.h"

/**
 * external declaration and other function delcared in other files
 */
extern symrec * symTable;
void yyerror(char *s);



linkedList * insertParam(linkedList ** lista, parameter * param){
    linkedList * item = (linkedList*) malloc(sizeof(linkedList));
    item->value.param = param;
    item->next = *lista;
    *lista = item;
    return *lista;
}

linkedList * invertParamOrder(linkedList * lista){
    linkedList * resLista = NULL;
    linkedList *item;
    parameter * temp;
    for (item=lista; item!=NULL; item=item->next) {
        temp = copyParam(item->value.param);
        resLista = insertParam(&resLista, temp);
    }

    return resLista;
}
linkedList * invertArgOrder(linkedList * lista){
    linkedList * resLista = NULL;
    linkedList *item;
    argument * temp;
    for (item=lista; item!=NULL; item=item->next) {
        temp = copyArgument(item->value.argument);
        resLista = insertArg(&resLista, temp);
    }
    
    return resLista;
}
linkedList * insertArg(linkedList ** lista, argument * argument){
    linkedList * item = (linkedList*) malloc(sizeof(linkedList));
    item->value.argument = argument;
    item->next = *lista;
    *lista = item;
    return *lista;
}
linkedList * insertRoutine(linkedList ** lista, struct routine * routine){
    linkedList * item = (linkedList*) malloc(sizeof(linkedList));
    item->value.routine = routine;
    item->next = *lista;
    *lista = item;
    return *lista;
}

void insertIntoList(linkedList ** lista, int value){
    linkedList * item = (linkedList*) malloc(sizeof(linkedList));
    item->value.val = value;
    item->next = *lista;
    *lista = item;
    
}
void readRoutines(linkedList * lista){
    printf("\t reading routine list\n");
    linkedList *item;
    for (item=lista; item!=NULL; item=item->next) {
        printf("%s \n", item->value.routine->name);
    }
}

routine * getRoutine(char const * name, linkedList * lista){
    linkedList *item;
    for (item=lista; item!=NULL; item=item->next) {
        int equals = strcmp(item->value.routine->name,name);
        if(equals == 0){
            return item->value.routine;
        }
    }
    return NULL;
}
int size(linkedList * lista){
    int count = 0;
    linkedList *item;
    for (item=lista; item!=NULL; item=item->next) {
        count++;
    }
    return count;
}

item * getItemAtIndex(linkedList * lista, int position){
    int count = 0;
    linkedList *item;
    for (item=lista; item!=NULL; item=item->next) {
        if(count == position){
            return item;
        }else{
            count++;
        }
    }
    return NULL;
}
symrec * populateEnv(linkedList* parameters, linkedList * args, symrec * currentSymTable){
    symrec * env = NULL;
    /*once here we have already checked lenght*/
    int paramsCount = size(parameters);
    for (int i = 0; i<paramsCount; i++) {
        item * arg;
        item * param;
        arg = getItemAtIndex(args ,i);
        param =getItemAtIndex(parameters, i);
        putsymTableValue(param->value.param->name, &env, ex(arg->value.argument, currentSymTable));
    }
    return env;
}


void readParams(linkedList * lista){
    printf("\t reading param list\n");
    linkedList *item;
    for (item=lista; item!=NULL; item=item->next) {
        printf("%s \n", item->value.param->name);
    }
}
void readArguments(linkedList* lista, symrec * tabella){
    printf("\t reading arg list\n");
    linkedList *item;
    for (item=lista; item!=NULL; item=item->next) {
        printf("\t%d \n", ex(item->value.argument, tabella));
    }
}
void readList(linkedList * lista){
    linkedList * item;
    for (item = lista; item!=NULL; item = item->next) {
        printf("%d\n", item->value.val);
    }
}

parameter * createParam(char * name, basicType type){
    parameter * res = (parameter*)malloc(sizeof(parameter));
    res->name = malloc(sizeof(strlen(name))+1);
    strcpy(res->name,name);
    res->type = type;
    return res;
}
parameter * copyParam(parameter * param){
    parameter * res = (parameter*)malloc(sizeof(parameter));
    res->name = malloc(sizeof(strlen(param->name))+1);
    strcpy(res->name,param->name);
    res->type = param->type;
    return res;
}
argument * copyArgument(argument * arg){
    argument * res = (argument*)malloc(sizeof(argument));
    res->type = arg->type;
    switch (res->type) {
        case typeCon:
            res->con=arg->con;
            break;
        case typeId:
            res->id=arg->id;
            break;
        case typeOpr:
            res->opr=arg->opr;
            break;
            
            
        default:
            printf("unsupported copyArgument exception\n program will exit now.\n");
            exit(0);
            break;
    }
    return res;
}
routine * createRoutine(char const * name, fpType routineType, basicType *returnType,
                        linkedList * parameterList, nodeType * commandList){
    routine * res = (routine*)malloc(sizeof(routine));
    res->name = malloc(sizeof(strlen(name))+1);
    strcpy(res->name, name);
    res->type = routineType;
    res->returnType = returnType;
    res->parameters = parameterList;
    res->commands = commandList;
    return res;
}


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

nodeType *id (symrec *ide){
    nodeType *p;
    if((p=malloc(sizeof(nodeType)))==NULL){
        yyerror("out of memory");
    }
    p->type = typeId;
    p->id.name=malloc(sizeof(strlen(ide->name))+1);
    strcpy(p->id.name, ide->name);
    if(getsym(p->id.name)==NULL){

        putsym(p->id.name);
    }
    return p;
}
nodeType *rout (char * name, linkedList * args){
    nodeType *p;
    if((p=malloc(sizeof(nodeType)))==NULL){
        yyerror("out of memory");
    }
    if((p->routine.args = malloc(sizeof(linkedList)))== NULL){
        yyerror("out of memory");
    }
    p->type = typeRoutine;
    p->routine.name=malloc(sizeof(strlen(name))+1);
    strcpy(p->routine.name, name);
    p->routine.args = args;
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
    va_start(ap, nops);
    for(i = 0; i<nops;i++){
        p->opr.op[i]=va_arg(ap,nodeType*);
    }
    va_end(ap);
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

symrec *
putsym (char const * identifier)
{
    symrec *ptr = (symrec *) malloc (sizeof (symrec));
    ptr->name = (char *) malloc (strlen (identifier) + 1);
    strcpy (ptr->name,identifier);
    ptr->next = (struct symrec *)symTable;
    symTable = ptr;
    return ptr;
}
symrec
*putsymTable(char const * identifier, symrec ** tabella){
    symrec *ptr = (symrec *) malloc (sizeof (symrec));
    ptr->name = (char *) malloc (strlen (identifier) + 1);
    strcpy (ptr->name,identifier);
    ptr->next = *tabella;
    *tabella = ptr;
    return ptr;
}
symrec
*putsymTableValue(char const * identifier, symrec ** tabella, int value){
    symrec *ptr = (symrec *) malloc (sizeof (symrec));
    ptr->name = malloc(strlen(identifier)+ 1);
    strcpy (ptr->name,identifier);
    ptr->value = value;
    ptr->next = *tabella;
    *tabella = ptr;
    return ptr;
}


symrec *
getsym (char const * identifier)
{
    symrec *ptr;
    for (ptr = symTable; ptr != (symrec *) 0;
         ptr = (symrec *)ptr->next){
        if (strcmp (ptr->name, identifier) == 0)
            return ptr;
    }
    return NULL;
}
void readTable(symrec * table){
    symrec *ptr;
    for (ptr = table; ptr != (symrec *) 0;
         ptr = (symrec *)ptr->next){
        printf("variable %s %d\n", ptr->name, ptr->value);
    }
}


symrec
*getsymTable(char const *identifier, symrec * tabella){
    symrec *ptr;
    for (ptr = tabella; ptr != (symrec *) 0;
         ptr = ptr->next){
        if (strcmp (ptr->name, identifier) == 0)
            return ptr;
    }
    return NULL;
}