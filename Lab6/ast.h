/*   Abstract syntax tree code


 Header file   
 Shaun Cooper January 2020

*/

#include<stdio.h>
#include<malloc.h>

#ifndef AST_H
#define AST_H
static int mydebug;

/* define the enumerated types for the AST.  THis is used to tell us what 
sort of production rule we came across */

enum AST_Tree_Element_Type {
   A_PACKAGE,
   A_METHODDEC,
   A_NUMBER,
   A_WHILESTMT,
   A_BLOCK,
   A_EXPR,
   A_PARAM,
   A_VARDEC

	   //missing
};


enum AST_Operators {
   A_PLUS,
   A_MINUS,
   A_TIMES,
   A_NOT
	   //missing
};

enum AST_Decaf_Types {
   A_Decaf_INT,
   A_Decaf_BOOL,
   A_Decaf_VOID
	   //missing
};

/* define a type AST node which will hold pointers to AST structs that will
   allow us to represent the parsed code 
*/
typedef struct ASTnodetype
{
     enum AST_Tree_Element_Type type;
     enum AST_Operators operator;
     char * name;
     int value;
     ///.. missing
     struct ASTnodetype *S1,*S2, *next ; /* used for holding IF and WHILE components -- not very descriptive */
} ASTnode;


/* uses malloc to create an ASTnode and passes back the heap address of the newley created node */
ASTnode *ASTCreateNode(enum AST_Tree_Element_Type mytype);

void PT(int howmany); // prints howmany spaces


ASTnode *program;  // Global Pointer for connection between YACC and backend

/*  Print out the abstract syntax tree */
void ASTprint(int level,ASTnode *p); // prints tree with level horizontal spaceing

#endif // of AST_H
