#
# Name: makefile
# Author: Courtney Mueller
# Date: 9 / 19 / 2022
# Purpose: this is a makefile to compile lab9
# Changes made: all files were named to fit files for lab9
#

all:	lab9

lab9:	lab9.l lab9.y ast.h ast.c symtable.c symtable.h emit.c emit.h 
	lex lab9.l
	yacc -d lab9.y
	gcc -lm -o lab9 lex.yy.c y.tab.c ast.c symtable.c emit.c

run: 
	./lab9

test:
	./lab9 < test.decaf
		
clean:	lab9
	rm -f ./lab9 lex.yy.c y.tab.c y.tab.h
