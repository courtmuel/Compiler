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

// new method called PT to account for indendation 
// precondition: must 
// postcondition: indentation is established depending on statement
/*  Helper function to print tabbing */
void PT(int howmany)
{
	for (int i = 0; i < howmany; i++)
		printf(" ");
}

// new method called AST_Print_Type to print out or tree types present
// precondition: must be one of the declared AST types
// postcondition: the AST type for given input 
void AST_Print_Type( enum AST_Decaf_Types t)
{
	switch (t) {

		case  A_Decaf_INT : printf(" INT ");
				    break;
		case  A_Decaf_BOOL : printf(" BOOL ");
				     break;
		case  A_Decaf_STRING : printf(" STRING ");
				       break;
		case A_Decaf_VOID : printf(" VOID ");
				    break;

          default:  fprintf(stderr,"Unknown AST DECAF TYPE !!!\n");
	} // of switch
}// of AST_Print_Type


// new method called ASTprint to print out our tree we created
// precondition: must have nodes established in tree 
// postcondition: AST tree that we created 
/*  Print out the abstract syntax tree */
void ASTprint(int level,ASTnode *p)
{
   int i;
   if (p == NULL ) return;
   else
     { 
       PT(level); /*indent */
       switch (p->type) 
       {
	case A_PROGRAM : printf("\n\nPROGRAM\n");
			 ASTprint(level + 1, p -> S1);
			 ASTprint(level + 1, p -> S2);
			 break;
	
	case A_PACKAGE : printf("PACKAGE %s \n", p -> name);
                         ASTprint(level + 1, p -> S1);
                         ASTprint(level + 1, p -> S2);
                         break;

	case A_EXTERN :  printf("EXTERN FUNC %s ", p -> name);
			 ASTprint(level, p -> S1);
 			 AST_Print_Type(p -> A_Declared_Type);
			 printf("\n");	 
			 break;

	case A_ExternType :  printf("\nEXTERN Type: ");
		             AST_Print_Type(p -> A_Declared_Type);
			     break;
       			
        case A_VARDEC :  printf("Variable ");
			 printf(" %s",p->name);
			 
			 if (p -> S1 != NULL) // then it is an array 
			 {
			 	printf("[");
			 	printf("%d",p -> S1 -> size);
			 	printf("]");
			 } // end if 
			 
			 printf(" offset ");
			 printf(" %d", p -> symbol -> offset);
			 printf(" with type ");
			 AST_Print_Type(p -> A_Declared_Type);

			 printf("\n");
			 break;
        
	case A_METHODDEC : printf (" METHOD FUNCTION ");
			   printf("%s with type ", p -> name);
                    	   AST_Print_Type(p -> A_Declared_Type);
                     	
			   /* print out the parameter list */
			   if (p->S1 == NULL ) 
		      
			   { PT(level+2); 
				   printf (" (NONE) "); }
			   else
			   { 
				   PT(level+2);
				   printf( " ( \n");
	 			   ASTprint(level+2, p->S1);
	  			   PT(level+2);
	  			   printf( ") ");
                           }
			   printf("\n");
			   
			   ASTprint(level+2, p->S2); // print out the blank
			   break;

	case A_METHODIDENTIFIER : printf("Method Variable ");
				  printf(" %s",p -> name);
				  
				  printf(" offset ");
			 	  printf(" %d", p -> symbol -> offset);
				  AST_Print_Type(p -> A_Declared_Type);
				  printf("\n");
				  break;		   

        case A_PARAM : printf("PARAMETER ");
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

        case A_EXPR : printf("EXPR ");
		      switch(p -> operator) {
			      case A_PLUS : printf(" + ");
                              break;

			      case A_MINUS : printf(" - ");
                              break;
			      
			      case A_TIMES : printf(" * ");
                              break;

			      case A_DIVIDE : printf(" / ");
                              break;
			      
			      case A_MOD : printf(" %% ");
                              break;	 

			      case A_AND : printf(" && ");
                              break;
			      
			      case A_OR : printf(" || ");
			      break;

			      case A_LEFTSHIFT : printf(" << ");
			      break;
			      
			      case A_RIGHTSHIFT : printf(" >> ");
			      break; 
	
			      case A_LEQ : printf(" <= ");
                              break;
			      
			      case A_LT : printf(" < ");
                              break;

		              case A_GE : printf(" >= ");
                              break;

			      case A_EQ : printf(" == ");
			      break;
			     
			      case A_NOT : printf(" ! ");
                              break;

			      case A_NE : printf(" != ");
                              break;

			      // greater than is not working yet 
			      case A_GT : printf(" > ");
			      break;
			      
			      default: printf(" unknown EXPR operator ");
		      }

		      printf("\n");
		      ASTprint(level + 1, p -> S1);
		      
		      if (p -> operator != A_NOT) 
			      ASTprint(level + 1, p -> S2);
                      break;

	case A_VAR_RVALUE : printf("Variable %s\n", p->name);
			    if (p->S1 != NULL) {
				    PT(level); printf("[\n");
				    ASTprint(level+1,p->S1);
				    PT(level); printf("]\n");
			    } // end if 
			    break;

	case A_LVALUE : printf("Variable %s \n", p -> name);
			    if (p -> S1 != NULL) {
				    PT(level); printf("[\n");
				    ASTprint(level+1, p -> S1);
				    PT(level); printf("]\n");
			    } // end if 
			    break;

	case A_BREAK : printf("BREAK STATEMENT  \n");
		       break;



	case A_RETURN : printf("RETURN STATEMENT  \n");
			ASTprint(level+1, p->S1); // do RETURN expr if any 
                        break;	       

        case A_BLOCK : printf("BLOCK STATEMENT  \n");
                       ASTprint(level+1, p->S1);
                       ASTprint(level+1, p->S2);
                       break;

        case A_WHILESTMT : printf("WHILE STATEMENT \n");
                           ASTprint(level+1, p->S1);
                           ASTprint(level+2, p->S2);
                           break;
                           
        case A_IFSTMT : printf("IF STATEMENT \n");
                           ASTprint(level+1, p -> S1);
                           ASTprint(level+2, p -> S2 -> S1);
                           
                           if(p -> S2 -> S2 != NULL)
                           {
                           	PT(level);
                           	printf("ELSE \n");
                           	ASTprint(level + 2, p -> S2 -> S2);
                           	printf("\n");
                            
                           } // end if 
                           break;

	case A_INTCONSTANT : printf("CONSTANT INTEGER %d\n",p->value);
                              break;

	case A_BOOLCONSTANT : printf("CONSTANT BOOLEAN ");
			       if (p -> value == 1) printf("true\n");
                                  else printf("false\n");
			       break;

	case A_ASSIGNSTMT : printf("ASSIGNMENT STATEMENT \n");
			    ASTprint(level + 1, p -> S1);
			    ASTprint(level + 1, p -> S2);
			    break;

	case A_METHODCALL : printf("METHOD CALL name: %s \n", p -> name);
			    PT(level+2);
		            printf( " ( \n");
			    ASTprint(level + 1, p -> S1);
			    PT(level + 2);
			    printf(" ) \n");
			    break;


	case A_STRINGCONSTANT : printf("STRING %s \n", p -> name);
				break;

	case A_FULLPARMLIST : printf("FULLPARMLIST \n");
			      ASTprint(level + 1, p -> S1);
			      break;

        default: printf("unknown type in ASTprint\n");


       }
       ASTprint(level, p->next);
     }

}

// new method called check_parameters to check formal parameters
// precondition: two parameters are established to be compared 
// postcondition: comparision between the two nodes 
int check_parameters(ASTnode *formal, ASTnode *actual)
{
	if ((formal == NULL) && (actual == NULL))
		return 1;
	if ((formal == NULL) || (actual == NULL))
		return 0;

	if (formal -> A_Declared_Type != actual -> A_Declared_Type)
		return 0;
	
	return (check_parameters(formal -> next, actual -> next));
} // end check_parameters


/* dummy main program so I can compile for syntax error independently   
 */

//void main()
//{
//}
/* */
