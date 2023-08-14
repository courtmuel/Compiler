
// Subprograms which output MIPS code.
//
// Shaun Cooper Spring 2015
// Updated Spring 2017 to utilize the data segement portion of MIPS instead of
// advancing the Stack and Global pointers to accomodate global variables.
// Shaun Cooper December 2020 updated for DECAF format

/*   
* Name: emit.c
* Author: Courtney Mueller
* Date: 12 / 1 / 2022 
* Purpose: this is a C program that contains our conversion code for decaf
*
* Changes made:
* 2022-11-13: create EMIT function 
* 2022-11-13: create EMIT_Strings function 
* 2022-11-13: create EMIT_Globals function 
* 2022-11-13: create variable for WSIZE and LOG_WSIZE
* 2022-11-13: define internal prototype and method Emit_Globals method
* 2022-11-14: define internal prototype and method Emit_AST method
* 2022-11-14: define internal prototype and method Emit_Method_Decl method
* 2022-11-15: define internal prototype and method Emit_Return method 
* 2022-11-15: define internal prototype and method Emit_Expr method
* 2022-11-15: define internal prototype and method Emit_Method_Call method
* 2022-11-20: define internal prototype and method Emit_Ident 
* 2022-11-20: define internal prototype and method Emit_If
* 2022-11-20: define internal prototype and method Emit_While 
* 2022-11-24: create global variable and method for temp labels 
* 2022-11-30: ensure there are pre and post conditions on all methods
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ast.h"
#include "emit.h"


// define the wordsize in bytes
#define WSIZE           8
#define LOG_WSIZE       3


int label = 0; // one up number for creating label 


// 
// function for temporary labels 
// pre-condition: pointer to temporary variables created for label generation
// post-condition: GAS code to create labels 
//
char * create_temp_label()
{
	char s[100];
	sprintf(s, "_L%d", label);
	label++; // increment temporary 
	return(strdup(s)); // return duplicate value 
} // end label create


// internal prototypes
void Emit_Globals(ASTnode * p, FILE * fp);
void Emit_AST(ASTnode * p, FILE * fp);
void Emit_Method_Decl(ASTnode * p, FILE * fp);
void Emit_Return(ASTnode * p, FILE * fp);
void Emit_Expr(ASTnode * p, FILE * fp);
void Emit_Method_Call(ASTnode * p, FILE * fp);
void Emit_Extern_Method_Call(ASTnode * p, FILE * fp);
void Emit_Assign(ASTnode * p, FILE * fp);
void Emit_Ident(ASTnode * p, FILE * fp);
void Emit_If(ASTnode * p, FILE * fp);
void Emit_While(ASTnode * p, FILE * fp);


// 
// helper function for pretty printing 
// pre-condition: label, action, comment
// post-condition: Clean GAS code in file 
//
void Emit_Print(FILE * fp, char * label, char * action, char * comment)
{
	if (strcmp (label, "") == 0) // NO label
	{
		if (strcmp, (action, "") == 0) // NO action NO label
			if (strcmp, (comment, "") == 0) // NO action NO label NO comment 
				fprintf (fp, "\n");
			else // NO action NO label YES comment 
				fprintf (fp, "\t\t# %s\n", comment);

		else // YES action NO label 
			if (strcmp (comment, "") == 0) // YES action NO label NO comment 
				fprintf (fp, "\t%s\t\n", action);
			else // YES action YES label YES comment 
				fprintf (fp, "\t%s\t# %s\n", action, comment);
	}

	else // YES label
	{
		if (strcmp (action, "") == 0) // NO action YES label 
			if (comment == NULL) // NO action YES label NO comment 
				fprintf(fp, "%s:\n", label);
			else // NO action YES label YES comment
				fprintf(fp, "%s:\t\t# %s\n", label, comment);
		else // YES action YES label 
			if (strcmp (comment, "") == 0) // YES action YES label NO comment 
				fprintf(fp, "%s:\t%s\t\n", label, action);
			else // YES action YES label YES comment 
				fprintf(fp, "%s:\t%s\t# %s\n", label, action, comment);
	} // end of label check
} // end Emit_Print


//
// pre-condition: pointer to AST tree and opened file
// post-condition: GAS code in fp that implements AST tree
//
void EMIT(ASTnode * p, FILE * fp)
{

	// print out the name of the package 
	fprintf(fp, "\n#	Package:   %s\n", p -> S2 -> name);
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	
	fprintf(fp, "\t .section .rodata # start of the RODATA section, strings first\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	
	// all of this is standard GAS header code 
	fprintf(fp, "_percentD:	.string \"%%d\" # ALWAYS needed for print int\n");
	fprintf(fp, "# NEXT lines are all the strings in the program\n");

	Emit_Strings(p, fp);
	fprintf(fp, "# END of print all the strings in the program\n\n");
	fprintf(fp, "\t.data # start of the DATA section for variables\n\n");
	fprintf(fp, "\t.comm _SCANFHOLD, 8, 8 # MANDITORY space for SCANF reads\n");

	Emit_Globals(p, fp);
	fprintf(fp, "# END of the DATA section for variables\n");
	
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	fprintf(fp, "\t.text	# start of CODE segment \n");

	fprintf(fp, "\n");
	Emit_AST(p, fp);

} // end EMIT


//
// pre-condition: pointer to ASTnode, and oen file. 
// post-condition: GAS code for GLOBAL variable to be placed into .data section
//
void Emit_Globals(ASTnode * p, FILE * fp)
{
	if (p == NULL) return;
	
	// if p -> is a var dec and is at level 0
	if ((p -> type == A_VARDEC) && (p -> symbol -> level == 0)) 
	{
		if (p -> S2 == NULL)
		{
			fprintf(fp, "\t.comm %s,%d,%d	# define a global variable\n", p -> name, p -> symbol -> mysize * WSIZE, p -> symbol -> mysize * WSIZE);
			fprintf(fp, "\t.align 8 \n");
		} // end if 
		
		else // we have initialization 
		{
			fprintf(fp, "%s:	.long %d # define a global variable with initial value\n", p -> name, p -> S2 -> value);
			fprintf(fp, "\t.align 8 \n");
		} // end else 
		
	}  // end of global variable match 
	
	Emit_Globals(p -> S1, fp);
	Emit_Globals(p -> S2, fp);
	Emit_Globals(p -> next, fp);

} // end Emit_Globals


//
// pre-condition: pointer to ASTnode and open FILE 
// post-condition: GAS code that generates all GAS elements for the rest of the tree
//		   other than the global variables and strings
//
void Emit_AST(ASTnode * p, FILE * fp)
{
	if (p == NULL) return;

	// switch statement to account for different types
	switch (p -> type)
	{
		case A_PACKAGE:		Emit_AST(p -> S1, fp);
					Emit_AST(p -> S2, fp);
					Emit_AST(p -> next, fp);
					break;
					
		case A_PROGRAM:		Emit_AST(p -> S1, fp);
					Emit_AST(p -> S2, fp);
					Emit_AST(p -> next, fp);
					break;
		
		case A_EXTERN:		break;

   		case A_ExternType:	break;


		case A_VARDEC:		break;
		
		case A_ASSIGNSTMT:	Emit_Assign(p, fp);
					Emit_AST(p -> next, fp);
					break;


		case A_METHODCALL:     	Emit_Method_Call(p, fp);
                                        Emit_AST(p -> next, fp);
                                        break;

		case A_BLOCK:		Emit_AST(p -> S2, fp);
					Emit_AST(p -> next, fp);
					break;

   		case A_METHODDEC:	Emit_Method_Decl(p, fp);
					Emit_AST(p -> next, fp);
					break;

		case A_RETURN:		Emit_Return(p, fp);
					Emit_AST(p -> next, fp);
					break;	
					
		case A_IFSTMT:		Emit_If(p, fp);
					Emit_AST(p -> next, fp);
					break;

		case A_WHILESTMT:	Emit_While(p, fp);
					Emit_AST(p -> next, fp);
					break;


		default: printf("WARNING WARNING WARNING. EMIT_AST SHOULD NEVER NEVER EVER BE HERE. FIX THIS NOW!!!\n");
	
	} // end switch 
} // end Emit_AST


//
// pre-condition: pointer to ASTnode, which points to Method Declaration as well as a pointer to open file
// post-condition: prints GAS code for the method
//
void Emit_Method_Decl(ASTnode * p, FILE * fp)
{
	char s[100];	
	
	// this is all standard GAS header code 
	fprintf(fp,"\t.globl %s \n", p -> name);
	fprintf(fp, "\t.type %s, @function \n", p -> name);
	Emit_Print(fp, p -> name, "", "Start of Function");
	fprintf(fp, " \n");
	fprintf(fp, "\t.cfi_startproc	# 	 STANDARD FUNCTION HEADER FOR GAS \n");
	fprintf(fp, "\tpushq   %rbp	# 	 STANDARD FUNCTION HEADER FOR GAS \n");
	fprintf(fp, "\t.cfi_def_cfa_offset 16	# 	 STANDARD FUNCTION HEADER FOR GAS \n");
	fprintf(fp, "\t.cfi_offset 6, -16	# 	 STANDARD FUNCTION HEADER FOR GAS \n");
	fprintf(fp, "\tmovq    %rsp, %rbp	# 	 STANDARD FUNCTION HEADER FOR GAS \n");
	fprintf(fp, "\t.cfi_def_cfa_register 6	# 	 STANDARD FUNCTION HEADER FOR GAS \n");
	
	// need to generate GAS code for the body of the function definition 
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	
	// carve out our space on the stack 
	sprintf(s, "subq $%d, %%rsp", p -> symbol -> mysize * WSIZE);
	Emit_Print(fp, "", s, "Carve out the stack for method.");
	

	// create ASTnode for method call 
	ASTnode * params = p -> symbol -> fparms;
	int counter = 8; // counter variable to account for bytes in registers 
	
	while (params != NULL)
	{
		sprintf(s, "mov $%d, %d(%%rsp)", params -> symbol -> offset * WSIZE);
		counter++; // increment counter 
		params = params -> next; // params is next connected 
	} // end while 
	
	Emit_AST(p -> S2, fp); // this does the body of the method
	Emit_Return(NULL, fp); // implicit return 

} // end Emit_Method_Decl


//
// pre-condition: pointer to ASTnode, which points to Return or NULL declaration
// post-condition: GAS code for the Return, %rax is 0 for NULL and set by Emit_Expr() for expression
//
void Emit_Return(ASTnode * p, FILE * fp)
{
	if (p == NULL || p -> S1 == NULL) 
		Emit_Print(fp, "", "mov $0, %rax", "\tDefault Return Value");
	else 
		Emit_Expr(p -> S1, fp); // pointer that points to return 


	Emit_Print(fp, "", "leave", "leave the function");
        Emit_Print(fp, "", ".cfi_def_cfa 7, 8", "STANDRD end function for GAS");
        Emit_Print(fp, "", "ret", "");

        Emit_Print(fp, "", "\n\t.cfi_endproc", "STANDRD end function for GAS");
        Emit_Print(fp, "", ".size   main, .-main", "STANDRD end function for GAS\n");

        Emit_Print(fp, "", ".ident  \"GCC: (SUSE Linux) 7.5.0\"", "");
        Emit_Print(fp, "", ".section        .note.GNU-stack,\"\",@progbits", "");
	fprintf(fp, "\n\n"); // add additional white space for readability

} // end Emit_Return


//
// pre-condition: pointer to ASTnode that is in the class or expressions - including CONSTANTS and MethodCall
// post-condition: GAS code that sets %rax to the evaluation of the expression
//
void Emit_Expr(ASTnode * p, FILE * fp)
{
	// we should NEVER get NULL here - due to the precondition 
	char s[100]; // holder to make dynamic commands 

		
	// base cases
	switch (p -> type) 
	{
		// right underneath one another to show they are treated same
		case A_INTCONSTANT:
		case A_BOOLCONSTANT:
			sprintf(s, "mov $%d, %%rax", p -> value);
			Emit_Print(fp, "", s, "EXPR load constant");
			return;
			break;

		case A_METHODCALL:
			Emit_Method_Call(p, fp);
			return; // %rax is set - so we just return 
			break;
		
		case A_VAR_RVALUE:
			Emit_Ident(p, fp); // RAX has memory address
			Emit_Print(fp, "", "mov (%rax), %rax", " read in memory value RVALUE");
			return;
			break;

					
		// fall through to the recursive case
		default: 
			break;

	} // end of base case switch
	
	
	Emit_Expr(p -> S1, fp); // evaluate the LHS Emit_Expr(p -> S1) 
	sprintf(s, "mov %%rax, %d(%%rsp)", p -> symbol -> offset * WSIZE);
	Emit_Print(fp, "", s, "STORE LHS of expression in memory"); // if there is RHS, save RAX into the temporary 
	

	Emit_Expr(p -> S2, fp);	// evaluate the RHS Emit_Expr(p -> S2)
	Emit_Print(fp, "", "mov %rax, %rbx", "right hand side needs to be set"); // MOVE RAX to RBX
	sprintf(s, "mov %d(%%rsp), %%rax", p -> symbol -> offset * WSIZE); 
	Emit_Print(fp, "", s, "fetch LHS of expression from memory"); // restore LHS to RAX
		
	
	// create a switch statement to go through all operations 		
	switch (p -> operator) 
	{
		case A_PLUS:	Emit_Print(fp, "", "add %rbx, %rax", "EXPR ADD");
		            	break;

		case A_MINUS:	Emit_Print(fp, "", "sub %rbx, %rax", "EXPR SUBTRACT");
				break;
		
		case A_TIMES:	Emit_Print(fp, "", "imul %rbx, %rax", "EXPR TIMES");
				break;

		case A_DIVIDE:	Emit_Print(fp, "", "mov $0, %rdx", "EXPR Upper Word 0 DIV");
				Emit_Print(fp, "", "idiv %rbx", "EXPR DIVIDE");
				break;

		case A_MOD:	Emit_Print(fp, "", "mov $0, %rdx", "EXPR Upper Word 0 DIV");
				Emit_Print(fp, "", "idiv %rbx", "EXPR MOD");
				Emit_Print(fp, "", "mov %rdx, %rax", "MOD needs to be in RAX");
				break;	
				
		case A_UMINUS:	Emit_Print(fp, "", "neg %rax", "EXPR UMINUS");
				break;
		
		case A_NOT:	Emit_Print(fp, "", "not %rax", "EXPR NOT '!'");
				break;
				
		case A_EQ:	Emit_Print(fp, "", "cmp %rbx, %rax", "EXPR Equal '=='");
				Emit_Print(fp, "", "sete %al", "EXPR Equal");
				Emit_Print(fp, "", "mov $1, %rbx", "Set RBX to 1 in order to filter RAX");
				Emit_Print(fp, "", "%rbx, $rax", "filter RAX");
				break;

		case A_LT:	Emit_Print(fp, "", "cmp %rbx, %rax", "EXPR LessThan");
				Emit_Print(fp, "", "setl %al", "EXPR LessThan");
				Emit_Print(fp, "", "mov $1, %rbx", "set rbx to one to filter rax");
				Emit_Print(fp, "", "and %rbx, %rax", "filter RAX");
				break;
		
		case A_LEQ:	Emit_Print(fp, "", "cmp %rbx, %rax", "EXPR LessEqual");
				Emit_Print(fp, "", "setle %al", "EXPR LessThan");
				Emit_Print(fp, "", "mov $1, %rbx", "set rbx to one to filter rax");
				Emit_Print(fp, "", "and %rbx, %rax", "filter RAX");
				break;


   		case A_GT:	Emit_Print(fp, "", "cmp %rbx, %rax", "EXPR Greaterthan");
				Emit_Print(fp, "", "setg %al", "EXPR LessThan");
				Emit_Print(fp, "", "mov $1, %rbx", "set rbx to one to filter rax");
				Emit_Print(fp, "", "and %rbx, %rax", "filter RAX");
				break;	
				
		case A_GE:	Emit_Print(fp, "", "cmp %rbx, %rax", "EXPR GE");
				Emit_Print(fp, "", "setge %al", "EXPR LessThan");
				Emit_Print(fp, "", "mov $1,%rbx", "set rbx to one to filter rax");
				Emit_Print(fp, "", "and %rbx, %rax", "filter RAX");
				break;
    		
		case A_NE:	Emit_Print(fp, "", "cmp %rbx, %rax", "EXPR Not Equal");
   				Emit_Print(fp, "", "setne %al", "EXPR LessThan");
				Emit_Print(fp, "", "mov $1,%rbx", "set rbx to one to filter rax");
				Emit_Print(fp, "", "and %rbx, %rax", "filter RAX");
				break;

		case A_AND:	Emit_Print(fp, "", "and %rbx, %rax", "EXPR AND");
				break;

		case A_OR:	Emit_Print(fp, "", "or %rbx, %rax", "EXPR OR");
				break;
			
			
		default: printf("Operator NOT implemented in expression\n"); // default case 
	
	} // end switch 
} // end Emit_Expr


// 
// pre-condition: pointer to ASTnode, which points to if statement
// post-condition: GAS code that implements if statement
// 
void Emit_If(ASTnode * p, FILE * fp)
{
	char s[100];
	
	char * label1 = create_temp_label();
	char * label2 = create_temp_label();

	// this accounts for the if portion of the if statement	
	Emit_Expr(p -> S1, fp);
	Emit_Print(fp, "", "cmp $0, %rax", "IF compare");
	sprintf(s, "JE %s", label1);
	Emit_Print(fp, "", s, "IF branch to else part");
	Emit_AST(p -> S2 -> S1, fp);
		
	// this accounts for the else portion of the if statement 
	sprintf(s, "JMP %s", label2);
	Emit_Print(fp, "", s, "IF branch past else part");
		
	Emit_Print(fp, label1, "", "ELSE target");
	Emit_AST(p -> S2 -> S2, fp);	
	Emit_Print(fp, label2, "", "End of IF");	

} // end Emit_If 


// 
// pre-condition: pointer to ASTnode, which points to while statement 
// post-condition: GAS code that implements while statement 
// 
void Emit_While(ASTnode * p, FILE * fp)
{
	char s[100];

	char * label1 = create_temp_label();
	char * label2 = create_temp_label();

	
	Emit_Print(fp, label1, "", "TOP of WHILE");
	Emit_Expr(p -> S1, fp);
	Emit_Print(fp, "", "cmp $0, %rax", "WHILE compare");
	sprintf(s, "JE %s", label2); // jump to label2
	Emit_Print(fp, "", s, "WHILE branch out");
	

	Emit_AST(p -> S2, fp);
	fprintf(fp, "\n");
	sprintf(s, "JMP %s", label1); // jump to label1
	Emit_Print(fp, "", s, "JUMP back to TOP WHILE");
	Emit_Print(fp, label2, "", "END of WHILE");
	 
	return;

} // end Emit_While 


//
// pre-condition: pointer to ASTnode, which points to Method 
// post-condition: GAS code for the Method, %rax is set by the method  - we run special code for External Methods
//
void Emit_Method_Call(ASTnode * p, FILE * fp)
{
	char s[100];
	
	
	// check to see if it is an EXTERN
	if (p -> symbol -> SubType = ID_Sub_Type_Extern);
	{
		Emit_Extern_Method_Call(p, fp);
		return;
	} // end if 
 	
	// if we are here we know that it is an INTERNAL method call
	// create a ASTnode params to loop through params for method calls 
	ASTnode * params = p -> S1; 
	
	// while params is not equal to NULL - execute while loop 
	while (params != NULL) 
	{
		Emit_Expr(params -> S1, fp); 
		sprintf(s, "mov %rax, %d(%rsp)", p -> symbol -> offset * WSIZE);
		params = params -> next;
	} // end while 
	
	
	fprintf(fp, "\tmov $%d, %%rax \t# expression a number", LOG_WSIZE);
	sprintf(s, "mov %rax, %d(%rsp)", p -> symbol -> offset * WSIZE);
	Emit_Print(fp, "", "", "stor arg value in our runtime stack");
	sprintf(s, "mov %d(%rsp), %rax", p -> symbol -> offset * WSIZE);
	Emit_Print(fp, "", "", "load calc argument into RAX");
        
        fprintf(fp, "mov $%d, %%rax \t# expression a number", LOG_WSIZE);
        sprintf(s, "mov %rax, %d(%rsp)", p -> symbol -> offset * WSIZE);
        Emit_Print(fp, "", "", "stor arg value in our runtime stack");
        sprintf(s, "mov %rax,  %d", p -> symbol -> offset * WSIZE);
        Emit_Print(fp, "", "", "store parameter value into appropriate registers");
        
	Emit_Print(fp, "", "", "# about to call a function, set up each parameter in the new activation record");
        Emit_Print(fp, "", "CALL me", "CALL to function");	

} // end Emit_Method_Call


// 
// pre-condition: pointer to ASTnode that points to Extern Method
// post-condition: GAS code for the Extern Method, %rax is set by the method and exits with error if we do not know
//
void Emit_Extern_Method_Call(ASTnode * p, FILE * fp)
{
	char s[100];
	
	// to print a string 
	if (strcmp ("print_string", p -> name) == 0)
	{
		sprintf(s, "\n\tmov $%s, %rdi", p -> S1 -> S1 -> label);
		Emit_Print(fp, "", s, " RDI is the label address");
	       	Emit_Print(fp, "", "mov $0, %rax", " RAX need to be zero");
	       	Emit_Print(fp, "", "call printf", " print a string");
	       	
		fprintf(fp, "\n");
		fprintf(fp, "\n");
	       	return;
	} // end if 
	
	// for printing an integer
	if (strcmp ("print_int", p -> name) == 0)
        {
		Emit_Expr(p -> S1 -> S1, fp);
		
		Emit_Print(fp, "", "", "PRINT INT");
		Emit_Print(fp, "", "mov %rax, %rsi", "RSI needs the value to print");
        	Emit_Print(fp, "", "mov $_percentD, %rdi", "RDI needs to be the print int format");
        	
        	Emit_Print(fp, "", "mov $0, %rax", "RAX needs to be 0 for printf");
        	Emit_Print(fp, "", "call printf", "print a number from expression");
		
		fprintf(fp, "\n");
		return;	
        } // end if
        
        // for scanning in an integer
	if (strcmp ("read_int", p -> name) == 0)
        {
		Emit_Print(fp, "", "mov $_SCANFHOLD, %rsi", "# read in a number"); 
		Emit_Print(fp, "", "mov $_percentD , %rdi", " rdi has integer format for scanf");
		Emit_Print(fp, "", "mov $0 , %rax", " No other parameters for scanf");
		Emit_Print(fp, "", "call  __isoc99_scanf", " call read");  
		Emit_Print(fp, "", "mov _SCANFHOLD, %rax", " bring value on STACK into RAX register for default value"); 
		
		fprintf(fp, "\n");
		return;	
        } // end if
        
} // end Emit_Extern_Method_Call


// 
// pre-condition: pointer to an ASSIGN
// post-condition: GAS code that implements following alogrithm:
//
void Emit_Assign(ASTnode * p, FILE * fp)
{
	char s[100];

	Emit_Expr(p -> S2, fp); // emit expression of S2
	sprintf(s, "mov %%rax, %d(%%rsp)", p -> symbol -> offset * WSIZE);
	Emit_Print(fp, "", s, "store RHS temporarily"); // store value of S2 in temp value

	Emit_Ident(p -> S1, fp); // emit address of LVALUE		
	sprintf(s, "mov %d(%rsp), %rbx", p -> symbol -> offset * WSIZE);
	Emit_Print(fp, "", s, " fetch RHS temporarily"); // extracts from memory the stored value
	Emit_Print(fp, "", "mov %rbx, (%rax)", " ASSIGN final store"); // stores extracted value in proper memory address
	
	fprintf(fp, "\n");
	return;	
	
} // end Emit_Assign


//
// pre-condition: pointer to an LVALUE OR RVALUE
// post-condition: GAS code to make %rax exact address of the Identifier 
// 		-- upstream claler may need to extract the value from memory
//
void Emit_Ident(ASTnode * p, FILE * fp)
{
	char s[100];

        // if it is an array then calculate the internal offset
        if (p -> S1 != NULL)
        {
                Emit_Expr(p -> S1, fp); // calls expr within brackets of array - value of expr is now in %rax
		Emit_Print(fp, "", "mov %rax, %rbx", "array component");
		fprintf(fp, "\tshl $%d, %%rbx \t# Multiply Wordsize for Array Reference", LOG_WSIZE);
		fprintf(fp, "\n");
        } // end if


        // need to check if it is a global
        if (p -> symbol -> level == 0)
        {
        	sprintf(s, "mov $%s, %%rax", p -> symbol -> name);
        	Emit_Print(fp, "", s, "Load Global variable address into RAX");
        } // end if
	 

       	else // if local - we need to load RAX with stack pointer plus offset
        {
        	sprintf(s, "mov $%d, %%rax", p -> symbol -> offset * WSIZE);
                Emit_Print(fp, "", s, "Gets identifier offset");
                Emit_Print(fp, "", "add %rsp, %rax", "we have direct reference to memory");
	} // end else
	 

        // if its an array we need to add rbx to rax
        if (p -> S1 != NULL)
        {
                Emit_Print(fp, "", "add %rbx, %rax", "add array offset to (Stack Pointer + Offset)");
        } // end if

} // end Emit_Ident


//
// pre-condition: pointer to ASTnode, and to open file. 
// post-condition: prints GAS code for all of the strings in the AST tree
//
void Emit_Strings(ASTnode * p, FILE * fp)
{
	if (p == NULL) 
		return;

	// if p -> is a STRINGCONSTANT print statement below
	if (p -> type == A_STRINGCONSTANT) 
	{
		fprintf(fp, "%s:	.string %s # GLOBAL STRING\n", p -> label, p -> name);
	}
	
	Emit_Strings(p -> S1, fp);
	Emit_Strings(p -> S2, fp);
	Emit_Strings(p -> next, fp);

} // end Emit_Strings
