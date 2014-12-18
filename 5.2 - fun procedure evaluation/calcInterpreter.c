#include <stdio.h>
#include <stdlib.h>
#include "calc.h"
#include "y.tab.h"

extern program * prg;

void executeProgram(program * program, symrec * symTable){
    ex(program->commandList, symTable);
    freeNode(program->commandList);
}

int ex(nodeType *p, symrec * symTable) {
    if (!p) return 0;
    switch(p->type) {
        case typeCon:       {
                                return p->con.value;
                            }
        case typeId:        {
                                symrec * s= getsymTable(p->id.name, symTable);
                                if( s == NULL){
                                    printf("there is not such %s varibale in the symtable\n", p->id.name);
                                    exit(0);
                                }
                                return s->value;
                            }
        case typeOpr:
            switch(p->opr.oper) {
                case WHILE:
                    while(ex(p->opr.op[0], symTable))
                    {
                        ex(p->opr.op[1], symTable);
                    }
                    return 0;
                case FOR:
                    {   symrec * s= getsymTable(p->opr.op[0]->id.name, symTable);
                        s->value = ex(p->opr.op[1], symTable);
                        while(s->value - ex(p->opr.op[2], symTable)){
                            ex(p->opr.op[3], symTable);
                            s->value++;
                        }
                        return 0;
                    }
                case IF:
                    if (ex(p->opr.op[0], symTable)){
                        
                        ex(p->opr.op[1], symTable);
                    }
                    else if (p->opr.nops > 2)
                    {
                        ex(p->opr.op[2], symTable);
                    }
                    return 0;

                
                case PRINT:     printf("%d\n", ex(p->opr.op[0], symTable)); return 0;
                case ';':       ex(p->opr.op[0], symTable); return ex(p->opr.op[1], symTable);
                case '=':       {   
                                    symrec * s= getsymTable(p->opr.op[0]->id.name, symTable);
                                    return s->value = ex(p->opr.op[1], symTable);
                                    }
                case UMINUS:    return -ex(p->opr.op[0], symTable);
                case '+':       return ex(p->opr.op[0], symTable) + ex(p->opr.op[1], symTable);
                case '-':       return ex(p->opr.op[0], symTable) - ex(p->opr.op[1], symTable);
                case '*':       return ex(p->opr.op[0], symTable) * ex(p->opr.op[1], symTable);
                case '/':       return ex(p->opr.op[0], symTable) / ex(p->opr.op[1], symTable);
                case '<':       return ex(p->opr.op[0], symTable) < ex(p->opr.op[1], symTable);
                case '>':       return ex(p->opr.op[0], symTable) > ex(p->opr.op[1], symTable);
                case GE:        return ex(p->opr.op[0], symTable) >= ex(p->opr.op[1], symTable);
                case LE:        return ex(p->opr.op[0], symTable) <= ex(p->opr.op[1], symTable);
                case NE:        return ex(p->opr.op[0], symTable) != ex(p->opr.op[1], symTable);
                case EQ:        return ex(p->opr.op[0], symTable) == ex(p->opr.op[1], symTable);
                    
                    
            }
        case typeRoutine:
                    {
                        /*look up routine in environment*/
                        routine * r = getRoutine(p->routine.name, prg->routineList);
                        if(r == NULL){
                            printf("No such %s in the environement\nProgram will quit now..", p->routine.name);
                            exit(0);
                        }
                        /*count number of parameters*/
                        int paramsCount = size(r->parameters);
                        /*count number of args*/
                        int argsCount = size(p->routine.args);
                        if(paramsCount!=argsCount){
                            printf("Parameter mismatch for %s call\nParams' list size is %d, provided args are %d\n"
                                   , p->routine.name, paramsCount, argsCount);
                            exit(0);
                        }
                        /*procede with params/args matching*/
                        symrec * environement = populateEnv(r->parameters, p->routine.args, symTable);
                        /*add the function name it self to the environment
                         we will be using it for evaluating the return type
                         */
                        symrec * fun = getsymTable(r->name, environement);
                        if (fun == NULL) {
                            putsymTable(r->name, &environement);
                        }
                        r->env = environement;
                        ex(r->commands, r->env);
                        if(r->type == PROCEDURE){
                            return 0;
                        }else{
                            /*look up variable in the environment*/
                            symrec * res = getsymTable(r->name, environement);
                            if(res == NULL){
                                printf("function %s does not return any type\n", r->name);
                            }
                            return res->value;
                        }

                    }
    }
    printf("%s\n", "Unable to evaluate node");
    return 0;
}

