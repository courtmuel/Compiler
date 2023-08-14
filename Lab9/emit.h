

//  Subprograms which output NASM code.
//
//   Shaun Cooper Spring 2017
/// Updated Spring 2017 to utilize the data segement portion of NASM instead of advancing the Stack and Global pointers
/// to accomodate global variables.

// NASM is called on linux as
//  gcc foo.s 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef EMIT_H
#define EMIT_H

#include "ast.h"
#include "symtable.h"

#define WSIZE		8  // number of bytes in a word
#define LOGWSIZE	3  // number of shifts to get to WSIZE

char * Create_Label();
FILE *fp;
void EMIT(ASTnode * p, FILE *fp);
void Emit_Strings(ASTnode * p, FILE * fp);
char * create_temp_label();

#endif  // of EMIT.h
