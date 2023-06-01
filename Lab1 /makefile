#
# Lab 1 -- simple lex mechanics, makefile that runs LEX and compiles LEX C code into lab1 everytime LEX file is updated
# Author: Courtney Mueller
# Date: August 24, 2022
#  

all: lab1 

lab1: wordlengthlab1.l
	lex wordlengthlab1.l 
	gcc lex.yy.c -o lab1

run: lab1
	./lab1 < /etc/passwd
