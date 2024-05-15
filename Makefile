program:  symtab.o  AsmCode.o AsmTreeGen.o lex.yy.o y.tab.o pl-0.o
		g++ -o pl-0 symtab.o  AsmCode.o AsmTreeGen.o lex.yy.o y.tab.o pl-0.o


symtab.o:			pl0-symtab.cpp 
		g++ -c pl0-symtab.cpp -o symtab.o

AsmTreeGen.o:			AsmTreeGen.cpp
		g++ -c AsmTreeGen.cpp -o AsmTreeGen.o

pl-0.o:			pl-0.cpp 
		g++ -c pl-0.cpp -g -o pl-0.o

AsmCode.o: AsmCode.h
		g++ -c AsmCode.cpp -o AsmCode.o

lex.yy.o: lex.yy.c
	g++ -c lex.yy.c   

y.tab.o: y.tab.c
	g++ -c y.tab.c

y.tab.c: pl-0-parser.y
	yacc -d pl-0-parser.y 

lex.yy.c: pl-0-scanner.l y.tab.c
	lex --header-file=lex.yy.h pl-0-scanner.l 

clean: 
	rm -fr lex.yy.* *.o y.tab.* y.h pl-0