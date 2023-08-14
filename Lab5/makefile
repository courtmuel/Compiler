#
# Name: makefile
# Author: Courtney Mueller
# Date: 9 / 25 / 2022
# Purpose: this is a makefile to compile lab5
# Changes made: copied all files from lab4 into folder for lab5 and change file names
# 		also ensure that makefile contains a clean executable
#

all:	lab5

lab5:		lab5.l lab5.y 
		lex lab5.l
		yacc -d lab5.y
		gcc -lm -o lab5 lex.yy.c y.tab.c 
		
clean:	lab5
		rm -f ./lab5
