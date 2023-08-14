/*
 * Name: symtable.c
 * Author: Courtney Mueller
 * Date: 9 / 18 / 2022
 * Purpose: this is a C program that implements a symbol table and its functions create, insert, etc...
 * Algorithm: the main data structure of this program is a Linked List
 *
 *
 * Recent changes made:
 * 2022-18-09 (Courtney Mueller): completely remove main from symtable.c because we no longer need a main here
 * 2022-18-09 (Courtney Mueller): create new function "FetchAddress" that will obtain the address of desired symbol
 * 2022-18-09 (Courtney Mueller): fix all loops and if statements that were missing inline statements
 * 2022-18-09 (Courtney Mueller): add pre-conditions and post-conditions to all methods
 */

/*
 * C program of the implementation of symbol table that can create, insert, modify, search, and display elements.
 * This code was pulled from: https://forgetcode.com/C/101-Symbol-table
 *
 */


#include <stdio.h>
#include "symtable.h"

int size = 0;
struct SymbTab *first,*last;

// completely remove main method from "symtable.c"

/**
 * creates a new node, sets its symbol and address to the given parameters, then adds the node to the list
 *
 * @param *s the symbol for the new entry
 * @param address the address for the new entry
 * @precondition table must be created and not have symbol inserted already
 * @postcondition table will have one more entry
 *
 */
void Insert(char *s, int address)
{
    int n;
    n = Search(s);

    // if symbol already exists within the table - print error message
    if (n == 1)
        printf("\n\tThe symbol already exists within the symbol table.\n\tDuplicate can't be inserted.");

    // else if symbol does NOT exist within table add it
    else 
    {
        struct SymbTab *p;
        p = malloc(sizeof(struct SymbTab));
        p -> symbol = strdup(s);
        p -> addr = address;
        p -> next = NULL;

        // if size of table is equal to 0
        if (size == 0)
        {
            first = p;
            last = p;
        } // end if
        else
        {
            last -> next = p;
            last = p;
        } // end else

        size++;
        printf("Symbol inserted\n");
    } // end else
} // end Insert()

/**
 * prints out the symbols and addresses of all the entries using a for loop
 *
 * @param none
 * @precondition theres a table to display
 * @postcondition displays a table when symbols are added
 */
void Display()
{
    int i;
    struct SymbTab *p;
    p = first;
    printf("\n\tSYMBOL\tADDRESS\n");

    // create loop to iterate through symbol table and print out contents in orderly fashion
    for(i = 0; i < size; i++)
    {
        printf("\t%s\t%d\n",p -> symbol, p -> addr);
        p = p -> next;
    } // end for
} // end of Display()

/**
 * creates new temporary Symbtab, traverses list using for loop, returns the temp Symbtab if desired symbol is found
 * returns NULL when symbol is not found.
 *
 * @param char *symbol, the symbol that is to be searched
 * @precondition the table must be populated
 * @postcondition NULL if symbol is not found, if symbol IS found return SymbTab
 */
int Search(char *s)
{
    int i, flag = 0;
    struct SymbTab *p;
    p = first;

    // for loop to traverse through list looking for desired symbol
    for(i = 0; i < size; i++)
    {
        // compare desired symbol with symbol in table
        if(strcmp(p -> symbol, s) == 0)
        {
            flag = 1;
            p = p -> next;
        }// end if
    }// end for

    return flag;
} // end of Search()

/**
 * deletes the given string from the list
 *
 * @param *s
 * @precondition list needs to populated otherwise Search() called within method will print error message and terminate
 * @postcondition list will have one less element - size will equal (size - 1)
 */
void Delete(char *s)
{
    struct SymbTab *p,*q;
    int a;
    q = NULL;
    p = first;
    a = Search(s);

    // check to see if symbol is there - if not print error message
    if(a == 0)
        printf("\n\tSymbol not found\n");

    else
    {
        // if symbol IS there
        while(strcmp(p -> symbol, s) != 0)
        {
            q = p;
            p = p -> next;
        } // end while

        // if q == NULL you are at the middle of list
        if (q == NULL)
            first = p -> next;

        // if p == last you are at then end of list
        else if(p == last)
        {
            last = q;
            last -> next = NULL;
        } // end else if

        else
        {
            p -> next = q -> next;
        } // end else

        size--;
        Display();
    } // end else
} // end Delete()     

/**
 * newly created method named "FetchAddress" that fetches address
 * of symbol being searched
 *
 * @param sym symbol
 * @preconditon symbol is present in the table
 * @postcondition returns assigned address
 */
int FetchAddress (char * sym)
{
    int i, address;
    struct SymbTab *p;
    p = first;

    // for loop that will traverse list looking for address of desired symbol in table
    for (i = 0; i < size; i++)
    {
        // if statement to compare strings
        if (strcmp(p -> symbol, sym) == 0)
            return (p -> addr);
    } // end for

    return address;
} // end FetchAddress