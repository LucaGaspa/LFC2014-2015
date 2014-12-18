
/** calculator calc.y
 * origin source by Tom Niemann at epaperpress.com/lexandyacc
 * revision Lorenzo Massimo Gramola (2014)
 */
typedef enum { typeCon, typeId, typeOpr, typeRoutine } nodeEnum; /* used in the struct nodeType
                                                     to define the type of node*/
typedef enum{ INTTYPE, FLOATTYPE} basicType;
typedef enum{ FUNCTION, PROCEDURE} fpType;
/* constants */
typedef struct {
    int value;                  /* value of constant */
} conNodeType;

/* identifiers */
typedef struct {
    char * name;
} idNodeType;
/*function and procedures*/
typedef struct{
    char * name;
    struct linkedList *args;
}routineNodeType;


/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag **op;	/* operands */
} oprNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */
    
    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
        routineNodeType routine; /*routine or procedure*/
    };
} nodeType;
typedef nodeType argument;
//=========================================
//||  ARGUMENT FUNCTIONS                 ||
//=========================================
argument * copyArgument(argument *);
//=========================================
//||  PROGRAM                            ||
//=========================================
typedef struct program{
    struct linkedList *routineList;
    nodeType * commandList;
    struct symrec * symtable;
}program;

typedef struct symrec
{
	char * name;
    int value;
	struct symrec *next;
} symrec;
//=========================================
//||      SYMREC FUNCTIONS               ||
//=========================================
symrec * getsym(char const *);
symrec * putsym (char const *);
symrec * putsymTable(char const *, symrec **);
symrec * putsymTableValue(char const *, symrec **, int);
symrec * getsymTable(char const *, symrec *);
void readTable(symrec *);
typedef struct linkedList {
    union{
        int val;
        struct parameter * param;
        struct routine * routine;
        argument * argument;
    } value;
    struct linkedList * next;
}linkedList;

typedef struct linkedList item;

//=========================================
//||      LINKED LIST FUNCTIONS          ||
//=========================================

void insertIntoList(linkedList **, int);
linkedList * insertParam(linkedList **, struct parameter *);
linkedList * insertRoutine(linkedList **, struct routine *);
linkedList * insertArg(linkedList **, argument *);

linkedList * invertParamOrder(linkedList *);
linkedList * invertArgOrder(linkedList *);

int size(linkedList *);
item * getItemAtIndex(linkedList *, int);

void readList(linkedList *);
void readParams(linkedList *);
void readArguments(linkedList*, symrec * );
void readRoutines(linkedList *);


//=========================================
//|| BEGIN PARAMETER STRUCTURE           ||
//=========================================
typedef struct parameter{
    char * name;
    basicType type;
}parameter;
//=========================================
//||  PARAMETER FUNCTIONS                ||
//=========================================
parameter * createParam(char *, basicType);
parameter * copyParam(parameter *);

//=========================================
//||    FUNCTIONS DEFINITIONS            ||
//=========================================
typedef struct routine{
    char * name;
    fpType type;
    basicType *returnType; //se procedure, basic type è vuoto
    linkedList * parameters;
    nodeType * commands;
    symrec * env;
}routine;

routine * createRoutine(char const *, fpType, basicType*, linkedList *, nodeType * );
routine * getRoutine(char const *, linkedList * );
symrec * populateEnv(linkedList*, linkedList *, symrec * );
//=========================================
//||   AUXILIARY FUNCTIONS               ||
//=========================================

 nodeType *opr(int oper, int nops, ...);
 nodeType *id(symrec *);
 nodeType *con(int );
 nodeType *rout(char *, linkedList *);

 void executeProgram(program *, symrec *);
 void freeNode(nodeType *);
 int ex(nodeType *p, symrec *);

