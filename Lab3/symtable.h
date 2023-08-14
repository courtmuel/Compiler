// 
// Name: symtable.h
// Author: Courtney Mueller
// Date: 9 / 7 / 2022
// Purpose: this is a header file...
//

// remove label
// change search to have one parameter - return 1 if there is 0 if not 
// change insert to have 2 parameters - label and address 
// change SymbTab struct to have char*
// insert does strdup of symbol with a malloc
// delete modify 

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>

#ifndef SYMTABLE_H
#define SYMTABLE_H

struct SymbTab {
    char * symbol;
    int addr;
    struct SymbTab *next;
};


void Insert(char *s, int address); // takes symbol and address and inserts if the symbol is not present
void Delete(char *s); // takes a symbol as input and removes if it is present
int Search(char *s); // takes a symbol as input and states (1) if present (0) if it is not
void Display();


#endif