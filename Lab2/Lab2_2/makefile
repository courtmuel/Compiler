#
# Name: makefile
# Author: Courtney Mueller
# Date: 9 / 1 / 2022
# Purpose: this is a makefile to compile lab2_2
#

# lab2_2 goes into detail on how LEX and YACC work together 
# LEX analyzes tokens decides what they are then feeds those tokens to YACC - when LEX sees "z" it feeds YACC a variable token
# when LEX analyzes an integer like "5" - it will feed YACC an integer token 
#
# a = 3 + 5 ... the sum of the two integers will be calculated and then stored in the register for a 
#

all:	lab2_2

lab2_2: lab2docalc.l lab2docalc.y
		lex lab2docalc.l
		yacc -d lab2docalc.y
		gcc y.tab.c lex.yy.c -o lab2_2

run:	lab2_2
		./lab2_2
