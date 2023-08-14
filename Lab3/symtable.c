/*
  * Name: symtable.c
  * Author: Courtney Mueller
  * Date: 9 / 11 / 2022
  * Purpose: this is a C program that implements a symbol table and its functions create, insert, etc...
  * Algorithm: the main data structure of this program is a Linked List
  *
  *
  * Changes made
  * 2022-05-09: code was properly indented and commented
  * 2022-05-09: included comments on all while, if and for statements
  * 2022-05-09: fixed spacing on all variable "=" and pointer declarations "->" to increase readability
  * 2022-05-09: also fixed spacing on all while, if and for statements / loops
  * 2022-05-09: fixed formatting and spelling of print statements
  * 2022-11-09: all instances of "label" were removed
  * 2022-11-09: return type of functions were changed
  * 2022-11-09: all interactions that involve user input (scanf) were removed from functions and moved to main
  * 2022-11-09: modify function was removed completely
  * 2022-11-09: any use of "strcpy" was changed to "strdup"
*/

/*
  * C program of the implementation of symbol table that can create, insert, modify, search, and display elements.
  * This code was pulled from: https://forgetcode.com/C/101-Symbol-table 
  *
*/


#include <stdio.h>
#include "symtable1.h"

int size = 0;
struct SymbTab *first,*last;

void main()
{
    struct SymbTab *y, *n, *a;
    int addr, op;
    char la[10], l[10];

    do
    {
        printf("\n\tSYMBOL TABLE IMPLEMENTATION\n");
        printf("\n\t1.INSERT\n\t2.DISPLAY\n\t3.DELETE\n\t4.SEARCH\n\t5.END\n");
        printf("\n\tEnter your option: ");
        scanf("%d",&op);

        switch(op)
        {
            case 1:
                printf("\n\tEnter the symbol: ");
                scanf("%s", l);

                if (Search(l) == 1)
                {
                    printf("\n\tThe symbol exists already in the symbol table\n\tDuplicate can't be inserted");
                } else {
                    printf("\n\tEnter the address: ");
                    scanf("%d", &addr);
                    Insert(l, addr);
                }
                break;

            case 2:
                Display();
                break;

            case 3:
                printf("\n\tEnter symbol to be deleted: ");
                scanf("%s", l);
                if (Search(l) == 0)
                    printf("\n\tLabel not found\n");
                else
                {
                    printf("\n\tAfter Deletion:\n");
                    Delete(l);
                } // end else
                break;

            case 4:
                printf("\n\tEnter the symbol to be searched: ");
                scanf("%s",la);

                printf("\n\tSearch Result:");

                if (Search(la) != 0)
                    printf("\n\tThe symbol is present in the symbol table\n");

                else
                    printf("\n\tThe symbol is not present in the symbol table\n");
                break;

            case 5:
                exit(0);
        }//end do

    }while(op<5);

}  /* end of main */

/**
 * creates a new node, sets its symbol and address to the given parameters, then adds the node to the list
 *
 * @param *s the symbol for the new entry
 * @param address the address for the new entry
 * @postcondition table will have one more entry
 *
 */
void Insert(char *s, int address)
{
    int n;
    n = Search(s);
    if (n == 1)
        printf("\n\tThe symbol already exists within the symbol table.\n\tDuplicate can't be inserted.");

    else {
        struct SymbTab *p;
        p = malloc(sizeof(struct SymbTab));
        p -> symbol = strdup(s);
        p -> addr = address;
        p -> next = NULL;
        if (size == 0)
        {
            first = p;
            last = p;
        }
        else
        {
            last -> next = p;
            last = p;
        } // end else

        size++;
        printf("\n\tSymbol inserted\n");
    }
} // end Insert()

/**
 * Prints out the symbols and addresses of all the entries using a for loop
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

    for(i = 0; i < size; i++)
    {
        printf("\t%s\t%d\n",p -> symbol, p -> addr);
        p = p -> next;
    }
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

    for(i = 0; i < size; i++)
    {
        if(strcmp(p -> symbol, s) == 0)
        {
            flag = 1;
            p = p -> next;
        }// end if
    }// end for

    return flag;
}// end of Search()

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

    // check to see if symbol is there
    if(a == 0)
        printf("\n\tSymbol not found\n");

    else
    {
        // if it is there
        while(strcmp(p -> symbol, s) != 0)
        {
            q = p;
            p = p -> next;
        } // end while

        if (q == NULL)
            first = p -> next;

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
    } // end of Delete()
} // end symtable
