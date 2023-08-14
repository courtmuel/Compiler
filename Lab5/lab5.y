%{

/*
* YACC Program
* Name: lab5.y
* Author: Courtney Mueller
* Date: 9  / 26 / 2022
* Purpose: the purpose of this YACC file is to create a set of tokens to represent everything we will be using in LEX 
	   as well as synatx directed semantic actions for each rule set that we create
*
* Recent changes made:
* 2022-09-26: change start to be Program
* 2022-09-26: add tokens for all variables used in our syntax directed semantic action 
* 2022-09-26: create a ruleset for all syntax directed semantic action (Program - MethodParam)
* 2022-09-26: create a "MethodParam" rule to represent method parameters
*
*/


// prototype to get rid of error message
int yylex();


	/* begin specs */
#include <stdio.h>
#include <ctype.h>
//#include "lex.yy.c"

// add new global variable to count line numbers 
int base, debugsw;
extern int lineCount;

// change made: update yyerror() to include the line number where an error occurs
void yyerror (s)  /* Called by yyparse on error */
     char *s;
{
  printf ("%s on Line number: %d\n", s, lineCount);
}

%} // end of C definitions


/*  defines the start symbol, what values come back from LEX and how the operators are associated  */
// 2022-09-26: change start to be Program
// 2022-09-26: add tokens for all variables used in program (YACC file)
// 2022-09-26: each defined rule has a ruleset (Program - MethodParam)
// 2022-09-26: some of the rulesets will contain syntax directed sematic action
// 2022-09-26: create a "MethodParam" rule to represent method parameters
%start Program

%union
{
    int value;
    char* string;
} // end UNION 

%token T_PACKAGE 
%token T_EXTERN
%token <string> T_ID
%token T_INT
%token T_FUNC 
%token T_BOOL 
%token T_STRING 
%token T_VOID
%token T_AND 
%token T_ASSIGN
%token T_BREAK
%token T_CONTINUE
%token T_DOT
%token T_ELSE
%token T_EQ
%token T_FALSE
%token T_FOR
%token T_GEQ
%token T_GT
%token T_IF
%token T_LEFTSHIFT
%token T_LEQ
%token T_NEQ
%token T_NULL
%token T_OR
%token T_RETURN
%token T_RIGHTSHIFT
%token T_TRUE
%token T_VAR
%token T_WHILE
%token T_STRINGCONSTANT
%token T_CHARCONSTANT
%token <value> T_INTCONSTANT 


%%	/* end specs, begin rules */

Program : Externs T_PACKAGE T_ID '{' FieldDecls MethodDecls '}'
	;

Externs : /* empty */
	| ExternDefn Externs
	;

ExternDefn  : T_EXTERN T_FUNC T_ID '(' ExternTypeList ')' MethodType ';'
            ;

ExternTypeList  : /* empty */
                | ExternTypeList1
                ;

ExternTypeList1 : ExternType
                | ExternTypeList1 ',' ExternType
                ;

ExternType  : T_STRING
	    | Type
	    ;


FieldDecls  : /* empty */
	    | FieldDecl FieldDecls
	    ;

FieldDecl   : T_VAR T_ID Type ';'
            ;

FieldDecl   : T_VAR T_ID ArrayType ';'
            ;

FieldDecl   : T_VAR T_ID Type T_ASSIGN Constant ';'
            ;

MethodDecls : /* empty */
            | MethodDecl MethodDecls
            ;

MethodDecl  : T_FUNC T_ID '(' MethodDeclParmList ')' MethodType Block
            ;

MethodDeclParmList  : /* empty */
                    | FullMethodDeclParmList
                    ;

FullMethodDeclParmList  : MethodParam
                        | FullMethodDeclParmList ',' MethodParam
                        ;


Block   : '{' VarDecls Statements '}'
        ;

VarDecls    : /* empty */
            | VarDecl VarDecls
            ;

VarDecl : T_VAR T_ID Type ';'
        ;

VarDecl : T_VAR T_ID ArrayType ';'
        ;

Statements  : /* empty */
            | Statement Statements
            ;

Statement   : Block
            ;

Statement   : Assign ';'
            ;

Assign  : Lvalue T_ASSIGN Expr
        ;

Lvalue  : T_ID
        | T_ID "[" Expr "]"
        ;

Statement   : MethodCall ';'
            ;

MethodCall  : T_ID '(' MethodCallParmList ')'
            ;

MethodCallParmList  : /* empty */
                    | FullMethodCallParmList
                    ;

FullMethodCallParmList  : MethodArg
                        | MethodArg ',' MethodCallParmList
                        ;

MethodArg   : Expr
            | T_STRINGCONSTANT
            ;

Statement   : T_IF '(' Expr ')' Block T_ELSE Block
	    | T_IF '(' Expr ')' Block 
            ;

Statement   : T_WHILE '(' Expr ')' T_IF Block T_ELSE
	    | T_WHILE '(' Expr ')' T_IF Block 
            | T_WHILE '(' Expr ')' Block 
	    ;	

Statement   : T_RETURN ';'
            | T_RETURN '(' ')' ';'
            | T_RETURN '(' Expr ')' ';'
            ;

Statement   : T_BREAK ';'
            ;

Statement   : T_CONTINUE ';'
            ;

Expr : Simpleexpression

Simpleexpression : Additiveexpression
                 | Simpleexpression Relop Additiveexpression
                 ;

Relop   : T_LEQ
        | '<'
        | '>'
        | T_GEQ
        | T_EQ
        | T_NEQ
        ;

Additiveexpression  : Term
                    | Additiveexpression Addop Term
                    ;
Addop   : '+'
        | '-'
        ;

Term : Factor
     | Term Multop Factor
     ;

Multop  : '*'
        | '/'
        | '%'
        | T_AND
        | T_OR
        | T_LEFTSHIFT
        | T_RIGHTSHIFT
        ;


Factor  : T_ID
        | MethodCall
        | T_ID'[' Expr ']'
        | Constant
        | '(' Expr ')'
        | '!' Factor
        | '-' Factor
        ;


BoolConstant    : T_TRUE
                | T_FALSE
                ;

ArrayType   : '[' T_INTCONSTANT ']' Type
            ;

Constant    : T_INTCONSTANT
            | BoolConstant
            ;

Type : T_INT
     | T_BOOL
     ; 
     
MethodType : T_VOID 
           | Type 
           ;

MethodParam : T_ID Type
            ;


%%	/* end of rules, start of program */

int main()
{ yyparse();
}
