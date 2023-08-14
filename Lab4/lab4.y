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



/*
*   Name: lab4.y
*   Author: Courtney Mueller
*   Date: 9  / 17 / 2022
*   Purpose: this is a YACC program that takes in integer and variable tokens from LEX and works as a calculator
*            the calculator consists of 26 integer registers to represent the letters in the alphabet
*            each letter in alphabet has a register within YACC using registers starting with 0
*
* Recent changes made:
* 2022-18-09 (Courtney Mueller): add in header file "symtable.h"
* 2022-18-09 (Courtney Mueller): remove all instances of "label" from programs present
* 2022-18-09 (Courtney Mueller): include statement to include in header file made for this program
* 2022-18-09 (Courtney Mueller): define the symbol table max size (suggested number was 26)
* 2022-18-09 (Courtney Mueller): set regs[] to the max size defined
* 2022-18-09 (Courtney Mueller): create a variable to represent the stack pointer and set it to 0
* 2022-18-09 (Courtney Mueller): create a variable to represent the base / head
* 2022-18-09 (Courtney Mueller): context free grammar of calculator was extended ( P, DECLS, DECL )
* 2022-18-09 (Courtney Mueller): start of program changed from "list" to "P"
* 2022-18-09 (Courtney Mueller): union type created to allow LEX and YACC to function together to return int or string
* 2022-18-09 (Courtney Mueller): define each token in CFG to be either of the types in the union (%type <value> expr)
* 2022-18-09 (Courtney Mueller): update the two previously declared tokens to have types (INTEGER, VARIABLE)
* 2022-18-09 (Courtney Mueller): within "DECL" create cases/error messages to be displayed when variable
                                 is already declared, when there is no register space, and to add to table
* 2022-18-09 (Courtney Mueller): within "stat" create cases/error messages to be display the answer, notify user
                                 when a variable has been declared, and when a variable has NOT been declared
* 2022-18-09 (Courtney Mueller): anytime regs[] is called - regs[FetchAddress($1)] is inserted to call function that
                                 was created in "symtable.c"
* 2022-18-09 (Courtney Mueller): add "VARIABLE" as an recognized expression
* 2022-18-09 (Courtney Mueller): add "INTEGER" as a recognized expression and print statement when integer is found
*/



	/* begin specs */
#include <stdio.h>
#include <ctype.h>

#include "symtable.h"
#define MAX 26


int regs[MAX];
int stackP = 0;
int base;

extern int debugsw;

int yylex();

void yyerror (s)  /* Called by yyparse on error */
     char *s;
{
  printf ("%s\n", s);
}


%}
/*  defines the start symbol, what values come back from LEX and how the operators are associated  */

%start P
%union 
{
	int value;
	char* string;
}

%token  <string> VARIABLE
%token <value> INTEGER
%token	T_INT
%type <value> expr

%left '|'
%left '&'
%left '+' '-'
%left '*' '/' '%'
%left UMINUS


%%	/* end specs, begin rules */

P	: DECLS list
	; 

DECLS	: DECLS DECL 
	| /* empty */
	;

DECL	: T_INT VARIABLE {
		    if (Search($2))
		    {
			    fprintf(stderr, "ERROR: Symbol %s is already defined.\n", $2);
		    } // end if

		    else
		    {
			    if (stackP >= MAX)
				    fprintf(stderr, "ERROR: No more register space in table!\n");
			
			    else
			    {
				    Insert($2, stackP);
				    stackP++;
			    } // end else
		    } // end else
	    } // END VARIABLE
';' '\n'
	;	

list	:	/* empty */
	|	list stat '\n'
	|	list error '\n'
			{ yyerrok; }
	;

stat	:	expr
			{ fprintf(stderr,"The answer is: %d\n", $1); }
	|	VARIABLE {
				if (Search($1)) {
					fprintf(stderr, "Variable %s is defined.\n", $1);
				}
				else
					fprintf(stderr, "Variable %s is NOT defined.\n", $1);
	} '=' expr 
			{ regs[FetchAddress($1)] = $4; } // FETCH ARRAY
	;


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
			{ $$ = regs[FetchAddress($1)]; }
	|	INTEGER {$$=$1; fprintf(stderr,"Found an integer\n");}
	;


%%	/* end of rules, start of program */

int main()
{ yyparse();
}
