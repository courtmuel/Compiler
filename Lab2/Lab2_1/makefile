#
# Name: lab2_1
# Author: Courtney Mueller
# Date: 9 / 3 / 2022
# Purpose: this is a makefile to compile and run lab2_1
#

all: lab2_1

lab2_1:	lab2remove.l
	lex lab2remove.l
	gcc lex.yy.c -o lab2_1

run: lab2_1
	./lab2_1 < comment.tst
