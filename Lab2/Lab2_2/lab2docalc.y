%{

/*
 *			**** CALC ****
 *
 * This routine will function like a desk calculator
 * There are 26 integer registers, named 'a' thru 'z'
 *
 */

/* This calculator depends on a LEX description which outputs either VARIABLE or INTEGER.
   The return type via yylval is integer 

   When we need to make yylval more complicated, we need to define a pointer type for yylval 
   and to instruct YACC to use a new type so that we can pass back better values
 
   The registers are based on 0, so we substract 'a' from each single letter we get.

   based on context, we have YACC do the correct memmory look up or the storage depending
   on position

   Shaun Cooper
    January 2015

   problems  fix unary minus, fix parenthesis, add multiplication
   problems  make it so that verbose is on and off with an input argument instead of compiled in
*/



/* Name: lab2docalc.y

   Author: Courtney Mueller

   Date: 9  / 3 / 2022

   Purpose: this is a YACC program that takes in integer and variable tokens from LEX and works as a calculator 
            the calculator consists of 26 integer registers to represent the letters in the alphabet
            each letter in alphabet has a register within YACC using registers starting with 0
*/



	/* begin specs */
#include <stdio.h>
#include <ctype.h>


int regs[26];
int base;

// 2022-01-09 (Courtney Mueller)... change made to "extern int debugsw"
extern int debugsw;

// 2022-02-09 (Courtney Mueller)... change made to add int yylex() to correct warning error during compilation
int yylex();

void yyerror (s)  /* Called by yyparse on error */
     char *s;
{
  printf ("%s\n", s);
}


%}
/*  defines the start symbol, what values come back from LEX and how the operators are associated  */

%start list

%token INTEGER
%token  VARIABLE

%left '|'
%left '&'
%left '+' '-'
%left '*' '/' '%'
%left UMINUS


%%	/* end specs, begin rules */

list	:	/* empty */
	|	list stat '\n'
	|	list error '\n'
			{ yyerrok; }
	;

stat	:	expr
     			// 2022-01-09 (Courtney Mueller)... fix spelling error in "answer", clean up print statement
			{ fprintf(stderr,"The answer is: %d\n", $1); }
	|	VARIABLE '=' expr
			{ regs[$1] = $3; }
	;

// 2022-03-09 (Courtney Mueller)... add rule to represent and calculate multiplication 
// 2022-01-09 (Courtney Mueller)... fix UNARY minus representation to be ('-' expression) not (expression '-' expression)
expr	:	'(' expr ')'
			{ $$ = $2; }
	|	expr '-' expr
			{ $$ = $1 - $3; }
	|	expr '+' expr
			{ $$ = $1 + $3; }
	|	expr '/' expr
			{ $$ = $1 / $3; }
	|	expr '%' expr
			{ $$ = $1 % $3; }
	|       expr '*' expr
                        { $$ = $1 * $3; }
	|	expr '&' expr
			{ $$ = $1 & $3; }
	|	expr '|' expr
			{ $$ = $1 | $3; }
	|	'-' expr	%prec UMINUS
			{ $$ = -$2; }
	|	VARIABLE
			{ $$ = regs[$1]; fprintf(stderr,"found a variable value =%d\n",$1); }
	|	INTEGER {$$=$1; fprintf(stderr,"found an integer\n");}
	;



%%	/* end of rules, start of program */

// 2022-01-09 (Courtney Mueller)... add return type to main to fix warning error during compilation 
int main()
{ yyparse();
}
