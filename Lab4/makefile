#
# Name: makefile
# Author: Courtney Mueller
# Date: 9 / 18 / 2022
# Purpose: this is a makefile to compile lab4
# Changes made: copied all files from lab2_2 / lab3 and change file names to fit this lab
# 				also made changes so that makefile contains a clean executable
#

all:	lab4

lab4:	symtable.c symtable.h lab4.l lab4.y 
		lex lab4.l
		yacc -d lab4.y
		gcc -o lab4 lex.yy.c y.tab.c symtable.c

clean:	lab4
		rm -f ./lab4
