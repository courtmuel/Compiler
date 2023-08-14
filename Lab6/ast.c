/*   Abstract syntax tree code

     This code is used to define an AST node, 
    routine for printing out the AST
    defining an enumerated type so we can figure out what we need to
    do with this.  The ENUM is basically going to be every non-terminal
    and terminal in our language.

    Shaun Cooper February 2020

*/

#include<stdio.h>
#include<malloc.h>
#include "ast.h" 


/* uses malloc to create an ASTnode and passes back the heap address of the newley created node */
ASTnode *ASTCreateNode(enum AST_Tree_Element_Type mytype)
{
    ASTnode *p;
    if (mydebug) fprintf(stderr,"Creating AST Node \n");
    p=(ASTnode *)malloc(sizeof(ASTnode)); // get head data
    p->type=mytype; // set up the Element type
    p->S1=NULL;
    p->S2=NULL;  // det default values
    p->value=0;
    return(p);
}

/*  Helper function to print tabbing */

void PT(int howmany)
{
	 // MISSING
}

void AST_Print_Type( enum AST_Decaf_Types t)
{
	switch (t) {

		case  A_Decaf_INT : printf(" INT ");
	                              break;

          default:  fprintf(stderr,"Unknown AST DECAF TYPE !!!\n");
	} // of switch


}// of AST_Print_Type


/*  Print out the abstract syntax tree */
void ASTprint(int level,ASTnode *p)
{
   int i;
   if (p == NULL ) return;
   else
     { 
       PT(level); /*indent */
       switch (p->type) {
        case A_VARDEC :  printf("Variable ");
                     if (p->operator == A_Decaf_VOID)
                       printf("VOID ");
                      printf(" %s",p->name);
                     if (p->value > 0)
                        printf("[%d]",p->value);
                     printf("\n");
		     ASTprint(level,p->S1);
                     break;
        case A_METHODDEC :  
                     if (p->operator == A_Decaf_INT)
                       printf("INT ");
                     if (p->operator == A_Decaf_VOID)
                       printf("VOID ");
                     if (p->operator == A_Decaf_BOOL)
                       printf("BOOLEAN ");
                     printf("FUNCTION %s \n",p->name);
                     /* print out the parameter list */
                     if (p->S1 == NULL ) 
		      { PT(level+2); 
		        printf (" (VOID) "); }
                     else
                       { 
                         PT(level+2);
                         printf( "( \n");
                          ASTprint(level+2, p->S1);
                         PT(level+2);
                         printf( ") ");
                       }
                     printf("\n");
                     ASTprint(level+2, p->S2); // print out the block
                     break;
        case A_PARAM :  printf("PARAMETER ");
                      if (p->operator == A_Decaf_INT)
                         printf (" INT ");
                      if (p->operator == A_Decaf_BOOL)
                         printf (" VOID ");
                      if (p->operator == A_Decaf_VOID)
                         printf (" BOOLEAN ");
                      printf (" %s",p->name);
                      if (p->value == -1 ) 
                         printf("[]");
                      printf("\n");                  
                     break;
        case A_EXPR :  printf("EXPR ");
                     switch(p->operator) {
   			case A_PLUS : printf(" + ");
                           break;
   			case A_MINUS: printf(" - ");
                           break;
                       }
                     printf("\n");
                     ASTprint(level+1, p->S1);
		     if (p->operator != A_NOT) 
                         ASTprint(level+1, p->S2);
                     break;
        case A_BLOCK :  printf("BLOCK STATEMENT  \n",p->name);
                     ASTprint(level+1, p->S1);
                     ASTprint(level+1, p->S2);
                     break;
        case A_WHILESTMT :  printf("WHILE STATEMENT \n");
                     ASTprint(level+1, p->S1);
                     ASTprint(level+2, p->S2);
                     break;
        case A_NUMBER :  printf("NUMBER with value %d\n",p->value);
                     ASTprint(level+1, p->S1);
                     break;
        default: printf("unknown type in ASTprint\n");


       }
       ASTprint(level, p->next);
     }

}



/* dummy main program so I can compile for syntax error independently   
 */
void main()
{
}
/* */
