#include <unistd.h> //sbrk

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef unsigned long ulong;

static ulong memorysize = 0;
static void* memstart;
static void* memend;
static ulong* pfree;
static ulong* pnext;
static ulong* plast;
static ulong* psectionsize;

/*
    Memory layout:
    at pfree        :   next memory section
    at pfree + 1    :   last memory section
    at pfree + 2    :   length of current section
*/

void movepointers() {
    plast = (ulong*)*pfree;
    pnext = (ulong*)*(pfree+8);
    psectionsize = (ulong*)*(pfree+16);
}

/*
    cases to handle
    1) first call, pfree doesnt exist yet
    2) pnext == plast
        mem sufficient
        mem insuffient
    3) pnext != plast
        mem sufficient
        mem insufficient
*/

void* stalloc( size_t size ) {

    //case 1
    if( memorysize == 0 ) {

        int togrow = 1024;

        while( size > togrow ) {
            togrow *= 1.2;
        }

        //setting all pointers to program endpoint, which should be the
        //the start of the data section 
        pfree = plast = pnext = memstart = sbrk(0);

        memend = sbrk( togrow );

        psectionsize = (ulong*)pfree+16;

        printf("%i %i\n", pfree, plast );

        //set value in first header section "last"
        //pfree as we have no mapped mem yet
        *plast = (ulong)&pfree;

        //set value in second header section "next"
        *pnext = (ulong)&pfree+8;

        //set value in third header section "sectionsize"
        *psectionsize = togrow;        
    }

    /*
    case 2: section size is sufficient 
            should probably happen after searching through the linked
            list and finding a section.
            set pfound to the current pointer
            pfree moves sizeof(header) + size to allocate forward in section
            pointers are set to the header positions
            and header data is written
    */
    
    ulong* pstart = plast;

    //search linked list until fitting section is found
    //or we are at the start again
    while( pstart != pfree && *psectionsize < size ) {
        pfree = ( ulong* )*(pfree+8);
        movepointers();
    }

    ulong* pfound = NULL;

    //if found something, set pfound and relink linked list
    if( *psectionsize >= size ) {
        pfound = pfree;

        pfree = pfree + 24 + size;
        movepointers();

        *plast = (ulong)&pfound;
        *pnext = (ulong)&pfound+8;
        *psectionsize = *(pfound+16) - size - 24;
    }

    //no pfound set means we need to get more memory
    if( pfound == NULL ) {
        pfound = memend+1;

        int togrow = 0;

        while( size + 24 > togrow ) {
            togrow *= memorysize * 1.2 - memorysize;
        }

        memend = sbrk( togrow );

        ulong* ptemp = pfree;

        pfree = pfound + 24 + size;
        movepointers();

        *plast = (ulong)&ptemp;
        *pnext = (ulong)&memstart;
        *psectionsize = (ulong)memend - size;
    }

    return pfound;
}

int main(int argc, char const *argv[])
{
    stalloc( 30 );
    return 0;
}