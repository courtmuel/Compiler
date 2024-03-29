/*                 Small LEX routine which returns two formal tokens (INTEGER and VARIABLE)
                   along with single string elements like '+'.
 
                   This LEX definition is the companion to the docalc.y YACC routine which
                   is a simple calculator

                   Shaun Cooper
                   January 2015
*/


/*
  *   Name:  lab4.l
  *   Author: Courtney Mueller
  *   Date: 9 / 17 / 2022
  *   Purpose: this program is a LEX file that accepts in integer and variable and single string elements
  *            the token is then processed in a seperate YACC file 'lab4.y' that works as a token calculator
  *
  * Changes made:
  * 2022-18-09 (Courtney Mueller): add rule for integers in which it returns int token created in lab4.y
  * 2022-18-09 (Courtney Mueller): add semi-colon to be accepted as symbol in rule 4
  * 2022-18-09 (Courtney Mueller): update LEX to match variable names instead of just single lower case characters
  * 2022-18-09 (Courtney Mueller): change "yylval" to be "yylval.string=strdup(yytext)"
  * 2022-18-09 (Courtney Mueller): change "yylval" for numbers to be "yylval.value="
  * 2022-18-09 (Courtney Mueller): in LEX, use strdup() to copy yytext to yylval.string so YACC can have
                                 a copy of variable name whenever VARIABLE matches
*/

%{

int mydebug = 1;
#include "y.tab.h"

%}


%%

int		{return (T_INT);}
[a-z][a-z0-9]*	{if (mydebug) fprintf(stderr,"Letter found\n");
                       yylval.string=strdup(yytext); return(VARIABLE);}
[0-9][0-9]*	{if (mydebug) fprintf(stderr,"Digit found\n");
                       yylval.value=atoi((const char *)yytext); return(INTEGER);}
[ \t]		{if (mydebug) fprintf(stderr,"Whitespace found\n");}
[;=\-+*/%&|()] 	{ if (mydebug) fprintf(stderr,"return a token %c\n",*yytext);
                       return (*yytext);}
\n 		{ if (mydebug) fprintf(stderr,"cariage return %c\n",*yytext);
                       return (*yytext);}

%%

int yywrap(void)
{ return 1;}
