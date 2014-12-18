
LEX = lex
YACC = yacc -d


CC = cc

interpreter: y.tab.o lex.yy.o calcInterpreter.o auxiliaryFunctions.o
	$(CC) -o interpreter y.tab.o lex.yy.o auxiliaryFunctions.o calcInterpreter.o -ll -lm 

calcInterpreter.o: calcInterpreter.c

auxiliaryFunctions.o: auxiliaryFunctions.c

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: calc.y
	$(YACC) -v calc.y

lex.yy.c: calc.l
	$(LEX) calc.l

clean:
	-rm -f *.o lex.yy.c *.tab.*  interpreter *.output
