%{

/*
* YACC Program
* Name: lab7.y
* Author: Courtney Mueller
* Date: 10 / 12 / 2022
* Purpose: this is a YACC program that reads in tokens from our LEX program and inserts it into
*	   our symbol, modified to now account for type checking and parameter checking 
*
* Recent changes made:
* 2022-10-12: include statement to import header file "symtable.h"
* 2022-10-12: include new global variable "LEVEL" to represent block levels
* 2022-10-12: include statement to import <stdlib.h> for exit statements
* 2022-10-12: add semantic action rules to BLOCK to account for entering / exiting a BLOCK
* 2022-10-12: modify VarDecl to include if statement to check for previously used T_ID
* 2022-10-16: create two new global variables OFFSET and GOFFSET
* 2022-10-17: modify usages of T_ID to include semantic action 
* 2022-10-17: exit within if statements allow program to function without an else statement
* 2022-10-20: create three new global variables - OFFSET, GOFFSET, and MAXOFFSET 
* 2022-10-30: include insert statements to all places where something should be added to symtable
*/


// prototype to get rid of error message
int yylex();


	/* begin specs */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ast.h"
#include "symtable.h" 

// global variable to count line numbers
extern int lineCount;

// introduce NEW global variable to represent level of BLOCKS we have seen 
int LEVEL = 0;

// AST global pointer that points to my structure 
struct ASTnodetype *PROGRAM;

// create three new global variables 
int OFFSET = 0;
int GOFFSET;
int MAXOFFSET;

// new method named yyerror() that prints when error occurs and the line # it happens on
// pre-condition: an error must have occured in order for yyerror() to be called
// post-condition: the error that occured + the line number it occurred 
void yyerror (s)  /* Called by yyparse on error */
     char *s;
{
  printf ("%s on line: %d\n", s, lineCount);
}

%} // end of C definitions


/*  defines the start symbol, what values come back from LEX and how the operators are associated  */
%start Program

%union
{
	int value;
	char* string;
	struct ASTnodetype *astnode;
	enum AST_Decaf_Types asttype;
	enum AST_Operators operator;
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
%token T_LT
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
%token <string>T_STRINGCONSTANT
%token T_CHARCONSTANT
%token <value> T_INTCONSTANT 

%type <astnode> Externs ExternDefn ExternTypeList1 ExternTypeList FieldDecl FieldDecls ArrayType MethodDecl MethodDecls
%type <astnode> MethodDeclParmList FullMethodDeclParmList Block VarDecl VarDecls 
%type <astnode> Statement Statements BreakStmt ReturnStmt IfStmt WhileStmt Assign ContinueStmt
%type <astnode> Factor Expr Term Additiveexpression Simpleexpression Lvalue MethodCall MethodCallParmList FullMethodCallParmList
%type <astnode> Constant BoolConstant MethodArg

%type <asttype> MethodType Type ExternType MethodParam 
%type <operator> Multop Addop Relop 


%%	/* end specs, begin rules */

Program : Externs T_PACKAGE T_ID '{' FieldDecls MethodDecls '}'
		{ PROGRAM = ASTCreateNode(A_PROGRAM);
		  PROGRAM -> S1 = $1;
		  PROGRAM -> S2 = ASTCreateNode(A_PACKAGE);
		  PROGRAM -> S2 -> name = $3;
		  PROGRAM -> S2 -> S1 = $5; // FieldDecls
		  PROGRAM -> S2 -> S2 = $6; // MethodDecls
		}	
	;

Externs : /* empty */
		{ $$ = NULL; }

	| ExternDefn Externs
		{ $1 -> next = $2; }
	;

ExternDefn  : T_EXTERN T_FUNC T_ID '(' ExternTypeList ')' MethodType ';'
            	{ 

		  // if statement check if $3 has already been defined. if so = error.
		  if (Search($3, LEVEL, 0) != NULL)
		  {
		  	yyerror("ERROR: Symbol has already been defined"); // throw error 
			yyerror($3); // display variable name
			exit (1); // exit out 
		  } // end if 

		  $$ = ASTCreateNode(A_EXTERN); 
	    	  $$ -> name = $3;
		  $$ -> A_Declared_Type = $7;
		  $$ -> S1 = $5;
		  $$ -> symbol = Insert($3, $7, ID_Sub_Type_Method, LEVEL, 0, 0, $5); 	  

		} 
	    ;

ExternTypeList  : /* empty */ { $$ = NULL; }
                | ExternTypeList1 { $$ = $1; };
                ;

ExternTypeList1 : ExternType
		   { $$ = ASTCreateNode(A_ExternType);
		     $$ -> A_Declared_Type = $1; 
		   }

                | ExternType ',' ExternTypeList1
                 { $$ = ASTCreateNode(A_ExternType);
                   $$ -> A_Declared_Type = $1;
		   $$ -> next = $3;
		 }
		;

ExternType  : T_STRING { $$ = A_Decaf_STRING; }
	    | Type 
	    ;


FieldDecls  : /* empty */ { $$ = NULL; } 
	    | FieldDecl FieldDecls
	      { $$ = $1;
 		$$ -> next = $2; 
	      }
	    ;

FieldDecl   : T_VAR T_ID Type ';'
	      { 
		if (Search($2, LEVEL, 0) != NULL) // means it is there
		{
			yyerror("Variable name is already used"); // throw error
			yyerror($2); // provide variable name 
			exit(1); // exit out
		} // end if 

		$$ = ASTCreateNode(A_VARDEC);
		$$ -> name = $2;
		$$ -> A_Declared_Type = $3;
		$$ -> symbol = Insert($2, $3, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
		OFFSET += 1;
	      }
	      
	    | T_VAR T_ID Type T_ASSIGN Constant ';'
	      { 
		if (Search($2, LEVEL, 0) != NULL)
		{
			yyerror("Variable name is already in use"); // throw error
			yyerror($2);
			exit(1);
		} // end if 

		$$ = ASTCreateNode(A_VARDEC);
		$$ -> name = $2;
		$$ -> A_Declared_Type = $3;
		$$ -> S2 = $5;
		$$ -> size = 1;
		$$ -> symbol = Insert($2, $3, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
		OFFSET += 1;
	      }

	    | T_VAR T_ID ArrayType ';'
	      { 
		if (Search($2, LEVEL, 0) != NULL)
		{
			yyerror("Variable name is not an array"); // throw error
			yyerror($2); // provide varaible name 
			exit(1); // exit out
		} // end if 

		$$ = ASTCreateNode(A_VARDEC);
		$$ -> name = $2;
		$$ -> S1 = $3; // array type created a VARDEC
		$$ -> A_Declared_Type = $3 -> A_Declared_Type;
		$$ -> size = $3 -> size; 
		$$ -> symbol = Insert($2, $$ -> A_Declared_Type, ID_Sub_Type_Array, LEVEL, $$ -> size, OFFSET, NULL);
		OFFSET += $$ -> size;
	      }
            ;

MethodDecls : /* empty */ { $$ = NULL; }
            | MethodDecl MethodDecls
            	{ $$ = $1;
		  $$ -> next = $2; 
		} 
	    ;	    


MethodDecl  : T_FUNC T_ID { GOFFSET = OFFSET;   OFFSET = 0; MAXOFFSET = OFFSET;} 
	      '(' MethodDeclParmList ')' MethodType 
		{
		if (Search($2, LEVEL, 0) != NULL)
                  {
                        yyerror("Method Name already used");
                        yyerror($2);
                        exit(1);
                  } // end if
		  Insert($2, $7, ID_Sub_Type_Method, LEVEL, 0, 0, $5);

		} 

		Block
            	{  
		  $$ = ASTCreateNode(A_METHODDEC);
		  $$ -> name = $2;
		  $$ -> A_Declared_Type = $7;
		  $$ -> S1 = $5; // change to $4 PARAM list
		  $$ -> S2 = $9; // change to $7 BlOCK
		  $$ -> symbol = Search($2, LEVEL, 0); 
		  $$ -> size = MAXOFFSET;
		  $$ -> symbol -> mysize = MAXOFFSET; 
		  OFFSET = GOFFSET;
		}
	    ;

MethodDeclParmList  : /* empty */ { $$ = NULL; }
                    | FullMethodDeclParmList { $$ = $1; }
                    ;

FullMethodDeclParmList  : T_ID MethodParam
			   { 
			     // check if it is at the level
			     if (Search($1, LEVEL + 1, 0) != NULL)
			     {			             
				yyerror("ERROR: Duplicate method arg name"); // throw error
				yyerror($1); // dusplay method arg
				exit(1);
		             } // end if 

			     $$ = ASTCreateNode(A_METHODIDENTIFIER);
			     $$ -> name = $1;
			     $$ -> A_Declared_Type = $2;
			     $$ -> symbol = Insert($1, $2, ID_Sub_Type_Scalar, LEVEL + 1, 1, OFFSET, NULL);
			     OFFSET += 1;
			   }

                        | T_ID MethodParam ',' FullMethodDeclParmList
			   { 
			     // check if it is at the level
                             if (Search($1, LEVEL + 1, 0) != NULL)
                             {
                                yyerror("ERROR: Duplicate method arg name"); // throw error
                                yyerror($1); // dusplay method arg
                                exit(1);
                             } // end if


			     $$ = ASTCreateNode(A_METHODIDENTIFIER);
			     $$ -> name = $1;
			     $$ -> A_Declared_Type = $2;
		             $$ -> next = $4;
			     $$ -> symbol = Insert($1, $2, ID_Sub_Type_Scalar, LEVEL + 1, 1, OFFSET, NULL);
			     OFFSET += 1;
			   }
                        ;

MethodParam : T_ID { A_Decaf_ID; }
	    | Type
            ;


Block   : '{'
	   	{ LEVEL++; } // anytime you ENTER a BLOCK - global variable increments by 1 
 	   VarDecls Statements '}'
	   
	   { $$ = ASTCreateNode(A_BLOCK);
	     $$ -> S1 = $3; // $2 when done
	     $$ -> S2 = $4; // $3 statements when done 
	     
	     Display();
  	     if (OFFSET > MAXOFFSET) MAXOFFSET = OFFSET;
	     OFFSET -= Delete(LEVEL);
             LEVEL--; // anytime you EXIT a BLOCK - global variable decrements by 1
	   }
        ;

VarDecls    : /* empty */ { $$ = NULL; } 
            
	    | VarDecl VarDecls
		{ $$ = $1;
		  $$ -> next = $2;
		}
            ;

VarDecl : T_VAR T_ID Type ';'
           { 
	     // if statement to check to see fT_ID has been used before
             if (Search($2, LEVEL, 0) != NULL) // means it is already there 
	     {
	     	yyerror("Variable already defined at this level."); // throw error
		yyerror($2); // display name of variable
		exit (1); // exit out 
	     } // end if 	     
 
	     $$ = ASTCreateNode(A_VARDEC);
	     $$ -> name = $2;
 	     $$ -> A_Declared_Type = $3;
	     $$ -> symbol = Insert($2, $3, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
	     OFFSET += 1;
	   }

	| T_VAR T_ID ArrayType ';'
	   { 
	     if (Search($2, LEVEL, 0) != NULL)
	     {
	     	yyerror("Variable name is not an array"); // throw error
		yyerror($2); // provide variable name 
		exit(1); // exit out
	     } // end if 	

	     $$ = ASTCreateNode(A_VARDEC);
	     $$ -> name = $2;
	     $$ -> S1 = $3; // array type created a VARDEC
	     $$ -> A_Declared_Type = $$ -> S1 -> A_Declared_Type;
	     $$ -> size = $3 -> size;
	     $$ -> symbol = Insert($2, $$ -> A_Declared_Type, ID_Sub_Type_Array, LEVEL, $$ -> size, OFFSET, NULL);
	     OFFSET += $$ -> size;
	   }
        ;

Statements  : /* empty */ { $$ = NULL; }
            | Statement Statements
		{ $$ = $1;
		  $$ -> next = $2;
		}
            ;

Statement   : Block { $$ = $1; }
	    | Assign ';' { $$ = $1; }
	    | MethodCall ';' { $$ = $1; }
            | IfStmt { $$ = $1; }
	    | WhileStmt { $$ = $1; }
	    | ReturnStmt { $$ = $1; }
	    | BreakStmt { $$ = $1; }
	    | ContinueStmt { $$ = $1; }
	    ;

Assign  : Lvalue T_ASSIGN Expr
	   { 
	     if ($1 -> A_Declared_Type != $3 -> A_Declared_Type)
	     {
	     	yyerror("Type mismatch on assignment ");
	     	exit(1);
	     }
	   
	     $$ = ASTCreateNode(A_ASSIGNSTMT);
	     $$ -> S1 = $1;
	     $$ -> S2 = $3;
	     $$ -> A_Declared_Type = $1 -> A_Declared_Type;
             $$ -> name = TEMP_CREATE();
	     $$ -> symbol = Insert($$ -> name, $$ -> A_Declared_Type, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
	     OFFSET++;
	   }
        ;

Lvalue  : T_ID
	   { 
	     struct SymbTab *p;
	     p = Search($1, LEVEL, 1);

	     // if statement to account for error handling 
	     if (p == NULL)
	     {
	     	yyerror("Symbol or LVALUE not defined"); // throw error
		yyerror($1); // display variable 
		exit(1); // exit out
	     } // end if 
	     
	     // if it is not at one of the subtypes
             if (p -> SubType != ID_Sub_Type_Scalar)    
   	     {
	     	yyerror($1); // throw error
		yyerror("Needs to be a scalar, WRONG SUBTYPE"); // display varaible 
		exit(1); // exit out
	     }	// end if      

	     $$ = ASTCreateNode(A_LVALUE);
	     $$ -> name = $1;
	     $$ -> symbol = p;
	     $$ -> A_Declared_Type = p -> Type;
	   }
        
	| T_ID '[' Expr ']'
           { 
	     struct SymbTab *p;
	     p = Search($1, LEVEL, 1);

	     // if statement to account for error handling
             if (p == NULL)
             {
                yyerror("Symbol or LVALUE not defined"); // throw error
                yyerror($1); // display variable
                exit(1); // exit out
             } // end if

	     // if it is not at one of the subtypes
             if (p -> SubType != ID_Sub_Type_Array)
             {
                yyerror($1); // throw error
                yyerror("Needs to be an array, WRONG SUBTYPE"); // display variable
                exit(1); // exit out
             }  // end if

	     $$ = ASTCreateNode(A_LVALUE);
             $$ -> name = $1;
             $$ -> S1 = $3; // means an array reference 
             $$ -> A_Declared_Type = p -> Type;
           }
        ;


MethodCall  : T_ID '(' MethodCallParmList ')'
	       { 
	       	 struct SymbTab *p;
		 p = Search($1, LEVEL, 1);

		 if (p == NULL)
		 {
		 	yyerror("Symbol not defined");
			yyerror($1);
			exit(1); 
		 } // end if 

		 if (p -> SubType != ID_Sub_Type_Method)
		 {
		 	yyerror($1);
			yyerror("Needs to be a method, wrong subtype");
			exit(1);
		 } // end if 
	       
		 // also need to check that the formal parameters 
		 // are the same length and type as the ACTUAL parameters
		 if (check_parameters(p -> fparms, $3) == 0)
		 {
		 	yyerror("Formal and actual parameters to not match");
			yyerror($1);
			exit(1);
		 } // end if 

	       	 $$ = ASTCreateNode(A_METHODCALL);
	         $$ -> S1 = $3;
	         $$ -> name = $1;
	         $$ -> symbol = p;
	         $$ -> A_Declared_Type = p -> Type;
	       }
            ;

MethodCallParmList  : /* empty */ { $$ = NULL; }
                    | FullMethodCallParmList { $$ = $1; }
                    ;

FullMethodCallParmList  : MethodArg
                           { $$ = ASTCreateNode(A_FULLPARMLIST);
                             $$ -> S1 = $1;
                             $$ -> next = NULL;
                             $$ -> name = TEMP_CREATE();
	       		     $$ -> symbol = Insert($$ -> name, A_Decaf_STRING, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
	                     OFFSET++;
	                     $$ -> A_Declared_Type = $1 -> A_Declared_Type;
                           }
                           
                        | MethodArg ',' MethodCallParmList
                           { $$ = ASTCreateNode(A_FULLPARMLIST);
                             $$ -> next = $3;
                             $$ -> S1 = $1;
                             $$ -> name = TEMP_CREATE();
	       		     $$ -> symbol = Insert($$ -> name, A_Decaf_STRING, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
	                     OFFSET++;
	                     $$ -> A_Declared_Type = $1 -> A_Declared_Type;
                           }
                        ;

MethodArg : Expr { $$ = $1; }
 	     

	  | T_STRINGCONSTANT
	    { $$ = ASTCreateNode(A_STRINGCONSTANT);
	       $$ -> name = $1; 
	       $$ -> A_Declared_Type = A_Decaf_STRING;
	    }
          ;

IfStmt : T_IF '(' Expr ')' Block
       	  { $$ = ASTCreateNode(A_IFSTMT);
	    $$ -> S1 = $3;
	    $$ -> S2 = ASTCreateNode(A_IFBODY);
	    $$ -> S2 -> S1 = $5;
	    $$ -> S2 -> S2 = NULL;
	  }

	| T_IF '(' Expr ')' Block T_ELSE Block
	  { $$ = ASTCreateNode(A_IFSTMT);
	    $$ -> S1 = $3;
	    $$ -> S2 = ASTCreateNode(A_IFBODY);
	    $$ -> S2 -> S1 = $5;
	    $$ -> S2 -> S2 = $7;
	  }
	
       ;

WhileStmt : T_WHILE '(' Expr ')' Block
	     { $$ = ASTCreateNode(A_WHILESTMT);
	       $$ -> S1 = $3;
	       $$ -> S2 = $5;
	     }
          ;
	

ReturnStmt : T_RETURN ';'
	      { $$ = ASTCreateNode(A_RETURN);
	      }
           | T_RETURN '(' ')' ';'
	      { $$ = ASTCreateNode(A_RETURN);
              }
           | T_RETURN '(' Expr ')' ';'
	      { $$ = ASTCreateNode(A_RETURN);
                $$ -> S1 = $3;
	      }
           ;

BreakStmt : T_BREAK ';'
	     { $$ = ASTCreateNode(A_BREAK);
	     }
          ;

ContinueStmt : T_CONTINUE ';'
	        { $$ = ASTCreateNode(A_CONTINUE);
	        }
             ;

Expr : Simpleexpression { $$ = $1; }

Simpleexpression : Additiveexpression { $$ = $1; }

                 | Simpleexpression Relop Additiveexpression
		   { 
		       if ($1 -> A_Declared_Type != $3 -> A_Declared_Type) 
		       { 
		       		yyerror("Both sides need to be the same type");
		       		exit(1);
		       } // end if 
		     
		       $$ = ASTCreateNode(A_EXPR);
	               $$ -> S1 = $1;
          	       $$ -> operator = $2;
          	       $$ -> A_Declared_Type = A_Decaf_BOOL;
          	       $$ -> S2 = $3;
          	       $$ -> name = TEMP_CREATE();
	  	       $$ -> symbol = Insert($$ -> name, A_Decaf_BOOL, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
	  	       OFFSET++;
        	   }
                 ;

Relop   : T_LEQ	{ $$ = A_LEQ;}
        | T_LT	{ $$ = A_LT; }
        | T_GT	{ $$ = A_GT; }
        | T_GEQ { $$ = A_GE; }
        | T_EQ	{ $$ = A_EQ; }
        | T_NEQ { $$ = A_NE; }
        ;

Additiveexpression  : Term { $$ = $1; }
                    | Additiveexpression Addop Term
			 { 
			   if (( $1 -> A_Declared_Type != $3 -> A_Declared_Type) ||
			       ($1 -> A_Declared_Type != A_Decaf_INT))
			   {
			   	yyerror("Addition and subtraction need Integers only");
			   	exit(1);
			   } // end if 
			   	
			 
			 
			   $$ = ASTCreateNode(A_EXPR);
         		   $$ -> S1 = $1;
          		   $$ -> operator = $2;
          		   $$ -> S2 = $3;
          		   $$ -> A_Declared_Type = $1 -> A_Declared_Type;
          		   $$ -> name = TEMP_CREATE();
	  		   $$ -> symbol = Insert($$ -> name, $$ -> A_Declared_Type, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
	  		   OFFSET++;
          		   
        		 }
                    ;

Addop   : '+'	{ $$ = A_PLUS; }
        | '-'	{ $$ = A_MINUS;}
        ;

Term : Factor { $$ = $1; }
     
     | Term Multop Factor
	{ 
	  if ($1 -> A_Declared_Type != $3 -> A_Declared_Type)
	  { yyerror("Type mismatch");
	    exit(1);
	  } // end if 
	  
	  if ( $1 -> A_Declared_Type == A_Decaf_BOOL && (($2 == A_TIMES) || 
	       ($2 == A_DIVIDE) ||
	       ($2 == A_MOD)))
	  {   yyerror("Cannot use Booleans in arithmetic operation");
	      exit(1);
	  } // end if 
	      
	
	  if ( $1 -> A_Declared_Type == A_Decaf_INT && (($2 == A_AND) || 
	       ($2 == A_OR) ||
	       ($2 == A_LEFTSHIFT) ||
	       ($2 == A_RIGHTSHIFT)))
	  {   yyerror("Cannot use Integer in boolean operation");
	      exit(1);
	  } // end if 
	  
	  $$ = ASTCreateNode(A_EXPR);
	  $$ -> S1 = $1;
	  $$ -> operator = $2;
	  $$ -> A_Declared_Type = $1 -> A_Declared_Type;
	  $$ -> S2 = $3;
	  $$ -> name = TEMP_CREATE();
	  $$ -> symbol = Insert($$ -> name, $$ -> A_Declared_Type, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
	  OFFSET++;
	}
     ;

Multop  : '*'	{ $$ = A_TIMES; }
        | '/'	{ $$ = A_DIVIDE;}
        | '%'	{ $$ = A_MOD;}
	| T_AND { $$ = A_AND;}
        | T_OR	{ $$ = A_OR; }
        | T_LEFTSHIFT	{ $$ = A_LEFTSHIFT; }
        | T_RIGHTSHIFT	{ $$ = A_RIGHTSHIFT; }
        ;


Factor  : MethodCall { $$ = $1; }
	
	| T_ID 
	   { 
	     struct SymbTab *p;
	     p = Search($1, LEVEL, 1);
	     
             if (p == NULL)
	     {
	     	yyerror("Symbol or RVALUE not defined"); // throw error
		yyerror($1); // display variable
		exit(1); // exit out 
	     } // end if 

             if (p -> SubType != ID_Sub_Type_Scalar)
	     {
		yyerror($1);
		yyerror("Needs to be a scalar, WRONG SUBTYPE");
		exit(1);
	     } // end if 

             $$ = ASTCreateNode(A_VAR_RVALUE);
	     $$ -> name = $1;
	     $$ -> symbol = p;
	     $$ -> A_Declared_Type = p -> Type;
	   }
        

	| T_ID'[' Expr ']'
	   { 
	     struct SymbTab *p;
             p = Search($1, LEVEL, 1);

             if (p == NULL)
             {
                yyerror("Symbol or RVALUE not defined"); // throw error
                yyerror($1); // display variable
                exit(1); // exit out
             } // end if

             if (p -> SubType != ID_Sub_Type_Array)
             {
                yyerror($1);
                yyerror("Needs to be an array, WRONG SUBTYPE");
                exit(1);
             } // end if

             $$ = ASTCreateNode(A_VAR_RVALUE);
             $$ -> name = $1;
	     $$ -> S1 = $3; // $1 is not NULL meaning its an array reference
             $$ -> symbol = p;
             $$ -> A_Declared_Type = p -> Type;
           }

        | Constant { $$ = $1; }
        
        | '(' Expr ')' { $$ = $2; }

        | '!' Factor
	   { 
	     if ($2 -> A_Declared_Type != A_Decaf_BOOL)
	     { yyerror ("Type mismatch, expected a boolean");
	     	  exit (1);
	     }
	   
	     $$ = ASTCreateNode(A_EXPR);
	     $$ -> operator = A_NOT;
	     $$ -> S1 = $2;
	     $$ -> A_Declared_Type = A_Decaf_BOOL;
	     $$ -> name = TEMP_CREATE();
	     $$ -> symbol = Insert($$ -> name, $$ -> A_Declared_Type, ID_Sub_Type_Scalar, LEVEL, 1, OFFSET, NULL);
	     OFFSET++;
	   }

        | '-' Factor
	   { $$ = ASTCreateNode(A_EXPR);
             $$ -> operator = A_UMINUS;
             $$ -> S1 = $2;
           }
        ;


BoolConstant    : T_TRUE
		   { $$ = ASTCreateNode(A_BOOLCONSTANT); 
		     $$ -> A_Declared_Type = A_Decaf_BOOL;
		     $$ -> value = 1; 
		   }
                | T_FALSE
                   { $$ = ASTCreateNode(A_BOOLCONSTANT); 
                     $$ -> A_Declared_Type = A_Decaf_BOOL;
		     $$ -> value = 0; 
		   }
                ;

ArrayType   : '[' T_INTCONSTANT ']' Type
		{ 
		  $$ = ASTCreateNode(A_VARDEC); 
		  $$ -> size = $2; 
		  $$ -> A_Declared_Type = $4;
		} ;

Constant    : T_INTCONSTANT 
	    	{ $$ = ASTCreateNode(A_INTCONSTANT); 
		  $$ -> value = $1; 
		}
            | BoolConstant { $$ = $1; } 
            ;

Type : T_INT { $$ = A_Decaf_INT; }
     | T_BOOL { $$ = A_Decaf_BOOL; }

MethodType : T_VOID { $$ = A_Decaf_VOID; }
	   | Type { $$ = $1; }
	   ;
%%	/* end of rules, start of program */


int main()
{ yyparse();
	Display();
	printf("Parsing completed\n");
	// we know that PROGRAM points to our AST
	
	ASTprint(0, PROGRAM);
	printf("\nFinished printing AST \n");
}
