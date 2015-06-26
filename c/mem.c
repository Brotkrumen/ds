#include <unistd.h> //sbrk

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef unsigned long ulong;

static ulong memorysize = 0;
static void* pmemstart;
static void* pmemend;
static ulong* pfree;
static ulong* pnext;
static ulong* plast;
static ulong* psectionsize;

#define HEADER_SIZE 3;

/*
    Memory layout:
    at pfree        :   next memory section
    at pfree + 1    :   last memory section
    at pfree + 2    :   length of current section
*/

void llnext() {
    plast = pfree;
    pfree = (ulong*)*pnext;
    pnext = (ulong*)*(pfree+1);
    psectionsize = (ulong*)*(pfree+2);
}

void addsize( ulong** pnew, ulong size ) {

    char* tmp = (char*)*pnew;
    tmp += size;
    *pnew = (ulong*)tmp;
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

void* stalloc( size_t insize ) {

    ulong size = (ulong)insize;

    printf( "requesting %i bytes\n", (int)size );

    //case 1
    if( memorysize == 0 ) {

        printf("first call to stalloc. getting mem\n");

        int togrow = 1024;

        while( size > togrow ) {
            togrow *= 1.2;
        }

        memorysize = togrow;

        //setting all pointers to program endpoint, which should be the
        //the start of the data section 
        pfree = plast = pmemstart = sbrk( togrow );

        //(ulong*)+1 moves ptr by sizeof(ulong*)*1 = 8 bytes
        pnext = pfree+1;
        psectionsize = pfree+2;

        pmemend = pmemstart + togrow;

        //set value in first header section "last"
        //pfree as we have no mapped mem yet
        *plast = (ulong)pfree;

        //set value in second header section "next"
        *pnext = (ulong)pfree;

        //set value in third header section "sectionsize"
        *psectionsize = togrow - 24;
        printf("--------------------- inital block size --------------------\n");
        printf( "    start  |   togrow   |       end\n%10i | %10i | %10i\n"
                ,(int)pfree, togrow, (int)pmemend ); 
        printf("------------------------------------------------------------\n\n");      
    }

    /*
    case 2: section size is sufficient 
            should probably happen after searching through the linked
            list and finding a sectinverseion.
            set pfound to the current pointer
            pfree moves sizeof(header) + size to allocate forward in section
            pointers are set to the header positions
            and header data is written
    */
    
    //set pstart to last element in linked list to see
    //when we ran a round
    ulong* pstart = (ulong*)*plast;

    //search linked list until fitting section is found
    //or we are at the start again
    printf("--------------------- looping through ll--------------------\n");
    printf("    pstart |     pfree  |sectionsize |       size |fits |isBeg\n" 
           "%10i | %10i | %10i | %10i | %3i | %3i\n"
            , (int)pstart,(int)pfree,(int)*psectionsize,(int)size
            , (int)( *psectionsize < size ), (int)(pstart != pfree));
    while( pstart != pfree && *psectionsize < size ) {
        printf("%10i | %10i | %10i | %10i | %3i | %3i\n"
                , (int)pstart,(int)pfree,(int)*psectionsize,(int)size
                , (int)( *psectionsize < size ), (int)(pstart != pfree));
        llnext();
    }
    printf("------------------------------------------------------------\n\n");

    ulong* pfound = NULL;

    //if found something, set pfound and relink linked list
    if( *psectionsize >= size ) {
        pfound = pfree;

        //move ptr by metadata size

        ulong* pnewfree = pfree + HEADER_SIZE;
        addsize( &pnewfree, size );

        printf("--------------------- found section case -------------------\n");
        printf("    pfound |      size  | size found |     pmoved\n"
                "%10i | %10i | %10i | %10i\n"
                ,(int)pfound, (int)size, (int)*psectionsize, (int)pnewfree );
        printf("------------------------------------------------------------\n\n");

        //if pfree == plast, only one element in free linked list
        //need to bind new plast pnext to the same new element
        if( pfree != plast ) {
            *pnewfree = (ulong)plast;
            *(pnewfree+1) = (ulong)pnext;
        } else {
            *pnewfree = (ulong)pnewfree;
            *(pnewfree+1) = (ulong)pnewfree;
        }
        
        *(pnewfree+2) = *psectionsize - 24 - size;

        pfree = pnewfree;
        plast = (ulong*)*pfree;
        pnext = (ulong*)*(pfree+1);
        psectionsize = (ulong*)*pfree+2;

        printf("  pfreenew |    plast  |     pnext  | sectionsize\n"
                "%10i | %10i | %10i | %10i\n"
                , (int)pfree, (int)plast, (int)pnext, (int)*psectionsize );
        printf("------------------------------------------------------------\n\n");
    }

    //no pfound set means we need to get more memory
    if( pfound == NULL ) {
        printf("--------------------- no fit found -----------------------\n");
        printf("--------------------- need more memory--------------------\n");
        printf("    memorysize |     reqested  \n" 
           "%10i | %10i\n", (int)memorysize, (int)size);
        printf("------------------------------------------------------------\n\n");

        pfound = pmemend;

        int togrow = memorysize * 1.2;

        if( size + 24 > togrow ) {
            togrow = size*1.2;
        }

        memorysize += togrow;

        printf("    togrow | new memsize  \n" 
                "%10i | %10i\n", (int)togrow, (int)memorysize);

        sbrk( togrow );
        pmemend += togrow;

        ulong* pnewfree = pfound;

        pnewfree += 3;
        addsize( &pnewfree, size );


        //TODO reserve function to avoid code doubling
        
        *pnewfree = (ulong)pfree;
        *(pnewfree+1) = *pnext;
        *(pnewfree+2) = *psectionsize - 24 - size;
        *pnext = (ulong)pnewfree;

        pfree = pnewfree;
        plast = (ulong*)*pfree;
        pnext = (ulong*)*(pfree+1);
        psectionsize = (ulong*)*pfree+2;
    }

    return pfound;
}

int main(int argc, char const *argv[])
{
    stalloc( 30 );
    stalloc( 2394 );
    return 0;
}