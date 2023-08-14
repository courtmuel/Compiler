/*                 Small LEX routine which returns two formal tokens (INTEGER and VARIABLE)
                   along with single string elements like '+'.
 
                   This LEX definition is the companion to the docalc.y YACC routine which
                   is a simple calculator

                   Shaun Cooper
                   January 2015
*/


/*
  * LEX program
  * Name:  lab6.l
  * Author: Courtney Mueller
  * Date: 09 / 26 / 2022 
  * Purpose: the purpose of this LEX file is to introduce tokens for values that we will be using in our YACC file 
  *
  * Changes made:
  * 2022-09-25: set "mydebug" to 0 to turn off debug statements
  * 2022-09-25: create a function HexToInt() that will convert hexadecimal strings to int
*/


%{

#include "y.tab.h"
#include <math.h>

int mydebug = 0;
int lineCount = 1;

//
// New method named "HexToInt" that will convert HEX values to integer values 
// pre-condition: the number is defined and is a HEX value
// post-condition: the number is defined and is a INTEGER (decimal) value 
//
int HexToInt ( char * hex )
{
	// define variables we need 
	int p = 0;
	int decimal = 0;
	int x, i;
	
	// for loop to iterate through and convert values 
	for (i = strlen(hex) - 1; i >= 0; --i)
	{	
		// if HEX values is greater than 0 and less than 9
		if (hex[i] >= '0' && hex[i] <= '9') 
		{
			x = hex[i] - '0';
		} else {
			x = hex[i] - 'A' + 10;
		} // end else 
		
		decimal = decimal + x * pow(16, p);
	} // end for 
	
	return decimal; // return our decimal converted value 

} // end HexToInt

%}


/* Begin Rules */
%%

\n      	{ lineCount++; }
"package"	{return (T_PACKAGE);}
"extern"	{return (T_EXTERN);}
"func"		{return (T_FUNC);}
"void"     	{ if (mydebug) fprintf(stderr, "void found\n"); return (T_VOID); }
"int"		{ if (mydebug) fprintf(stderr, "int found\n"); return (T_INT); }
"string"	{ if (mydebug) fprintf(stderr, "string found\n"); return (T_STRING); } 
"bool"		{ if (mydebug) fprintf(stderr, "bool found\n");return (T_BOOL); }
"&&"		{ return (T_AND);}
"="		{return(T_ASSIGN);}
"break"		{return(T_BREAK);}
"continue" 	{return(T_CONTINUE);}
\.		{return(T_DOT);}
"else"		{ if (mydebug) fprintf(stderr, "else found\n"); return(T_ELSE); } 
"=="		{return(T_EQ);}
"false"		{return(T_FALSE);}
"for"		{return(T_FOR);}
">="		{return(T_GEQ);}
">"		{return(T_GT);}
"if"		{ if (mydebug) fprintf(stderr, "if found\n"); return(T_IF); }
"<<"		{return(T_LEFTSHIFT);}
"<="		{return(T_LEQ);}
"!="		{return(T_NEQ);}
"null"		{return(T_NULL);}
"||"		{return(T_OR);}
"return"	{return(T_RETURN);}
">>"		{return(T_RIGHTSHIFT);}
"true"		{return(T_TRUE);}
"var"		{return(T_VAR);}
"while"		{ if (mydebug) fprintf(stderr, "while found\n"); return(T_WHILE); } 


\'.\'       {yylval.value=yytext[1]; return(T_CHARCONSTANT);}
\".*\"      {yylval.string=strdup(yytext); return(T_STRINGCONSTANT);}

[a-zA-Z][a-zA-Z0-9_]*   {if (mydebug) fprintf(stderr,"Letter found\n");
                            yylval.string=strdup(yytext); return(T_ID);}

[0-9][0-9]*     {if (mydebug) fprintf(stderr,"Digit found\n");
                    yylval.value=atoi((const char *)yytext); return(T_INTCONSTANT);}

[ \t]       // {if (mydebug) fprintf(stderr,"Whitespace found\n");}


[!<>()\-+*/%&\[\]|;{},]       { if (mydebug) fprintf(stderr,"return a token %c\n",*yytext);
                            return (*yytext);}
            
0x[0-9a-fA-F]+	{ if (mydebug) fprintf(stderr, "hex found\n");
                    yylval.value=HexToInt((char *)yytext); return(T_INTCONSTANT);}
\/\/.* 		{ /* this is a comment line for double slash  Return nothing, keep going */ }		

%%

int yywrap(void)
{ return 1;}
