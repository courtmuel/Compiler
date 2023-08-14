// 
// Name: symtable.h
// Author: Courtney Mueller
// Date: 9 / 18 / 2022
// Purpose: this is a header file for "symtable.c" and lab4
//

// 2022-05-09 (Courtney Mueller): remove label
// 2022-05-09 (Courtney Mueller): change search to have one parameter - return 1 if there is 0 if not
// 2022-05-09 (Courtney Mueller): change insert to have 2 parameters - label and address
// 2022-05-09 (Courtney Mueller): change SymbTab struct to have char*
// 2022-05-09 (Courtney Mueller): insert strdup of symbol with a malloc
// 2022-05-09 (Courtney Mueller): delete modify method completely

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
void Display(); // deletes desired symbol from table
int FetchAddress(char * symbol); // fetches assigned address for desire symbol(s)


#endif
