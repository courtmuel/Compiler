#
# Name: makefile
# Author: Courtney Mueller
# Date: 10 / 2 / 2022
# Purpose: this is a makefile to compile lab6
# Changes made: get all important files from lab5 and rename accordingly 
	        create DECAF with an abstract syntax tree 
#

all:	lab5

lab6:		lab6.l lab6.y ast.c ast.h 
		lex lab6.l
		yacc -d lab6.y
		gcc -o lab6 lex.yy.c y.tab.c ast.c 
		
clean:	lab6
		rm -f ./lab6
