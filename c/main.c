#include <stdio.h>
#include <stdlib.h>

//TODO:
//validate input
//check if input token grows the string and grow it
//check if container grows stack or harbor and grow them

//should be unsigned 32bit int
typedef unsigned long int uint;

const uint INITIAL_STACK_SIZE = 50;
const uint INITIAL_HARBOR_SIZE = 10;

/******************************************************************************
* Description	bookkeeping struct for one container stack.
*
* members:
*	length		length of the allocated array
*	last		last valid entry in the array
*	array*		pointer to first element
*				holds uint values
*
/******************************************************************************/

struct stack_t {
	uint length;
	uint last;
	uint* array;
};


/******************************************************************************
* Description	bookkeeping struct for the "harbor".
*				array of stack_t structs. equivalent to how many stacks
*				of containers exist
*
* members:
*	length		length of the allocated array of stack_t*
*	last		last valid entry in the array
*	array*		pointer to first element
*				holds struct stack_t pointers
*
/******************************************************************************/

struct stackvec_t {
	uint length;
	uint last;
	struct stack_t** array;
};


/******************************************************************************
* Description	frees the allocated memory of the struct stack_t
*				calls free on the array and then on the struct
*
* param			pointer to stack
*
/******************************************************************************/

void freestack(struct stack_t* pStack) {

	free(pStack->array);
	free(pStack);

}

/******************************************************************************
* Description	frees the allocated memory of the struct stackvec_t or "harbor"
*				goes through all array members and calls free on them and then
*				on the struct
*
* param			pointer to "harbor"
*
/******************************************************************************/

void freestackvec(struct stackvec_t* pHarbour) {

	for (int i = 0; i < pHarbour->last; i++) {
		freestack( &(pHarbour->array[i]) );
	}

	free(pHarbour->array);
	free(pHarbour);

}

/******************************************************************************
* Description	prints the stackvec_t, the harbor
*
* Format:		$stack_count $space $stack $space $stack ...
*
* param			pointer to "harbor"
*
/******************************************************************************/

void printstack(struct stackvec_t* pHarbor) {

	printf( (pHarbor->last + 1) + ' ' );

	struct stack_t* pStack;
	for (uint itHarbor = 0; itHarbor <= pHarbor->last; itHarbor++) {
		pStack = pHarbor->array[itHarbor];
		for (uint itStack = 0; itStack <= pStack->last; itStack++) {
			
			if (itStack != 0) printf(',');

			printf(pStack->array[itStack]);
		}
	}
}

/******************************************************************************
* Description	initializes the struct stackvec_t, the harbor.
*				allocates a stack to calculate the size of the necessary alloc
*				to avoid having to do that manually
*				its a bold move cotton, lets see if it works out.
*				
*
* returns		pointer to struct stackvec_t
*
/******************************************************************************/

struct stackvec_t* init() {

	//ptr to stack
	struct stack_t* pStack = (struct stack_t*) malloc(sizeof(struct stack_t));
	
	//set array of container length and allocate mem
	pStack->length = 50;
	uint* array[] = (uint*)malloc(sizeof(uint) * INITIAL_STACK_SIZE );

	//assign pointer of the array to the stack
	pStack->array = array;

	//set first valid entry position
	pStack->last = 0;

	//ptr to vector holding all stacks
	struct stackvec_t* pHarbor = (struct stackvec*) malloc(sizeof(pStack) * 10);

	pHarbor->last = 0;
	pHarbor->length = 10;

	freestack(pStack);

	return pHarbor;
}

/******************************************************************************
* Description	inserts the number "container" into the appropriate stack
*				if the harbor is overflowing, calls grow()
*				if the stack the newContainer is to be placed on is overflowing,
*				calls grow on that.
*				
* Algorithm:	1)	go through harbor and find the stack with the highest
*					container as the last element to give lastContainerOnBestStack
*					a proper initial value for later evaluation
*					if it is the best stack, newContainer will be put on it,
*					if not, the other stacks can beat the value
*					(might actually just use the the highest possible value as
*					initial, as i think twas just a hack to use auto in cpp as
*					in the stack pointer)
*
*				2)	check each stack if the newContainer is either equal to the
*					one on top of the stack
*						if yes, put it on it and next. best case
*					check if this stack can hold the container and is the best
*					stack to do that so far
*						if yes, save the stack position
*
*				3)	if one of the two cases in 2) have been found, put the
*					newContainer on it
*					if not, make a new stack
*
*				4)	if no new containers are coming, aka newline, prints and
*					discards harbor. makes a new one
*
*
* returns		pointer to struct stackvec_t
*
/******************************************************************************/

void addtostack(uint container, struct stackvec_t* pHarbor) {

	uint max = 0 - 1;

	for (uint itHarbor = 0; itHarbor != pHarbor->last; itHarbor++) {

		//TODO
		//get highest

	}


	if (pHarbor->length == pHarbor->last) {
		//TODO
		//grow pHarbor
	}

	

}

char* grow(chararray) {

	const int newsize = sizeof(chararray) * 1.2;

	char* newchararray[] = malloc( newsize );

	strcpy(newchararray, chararray);

	return newchararray;

}

void strappend(char c, char* chararray[]) {

	if (strlen(chararray) == sizeof(chararray) / sizeof(char) - 1) {
		chararray = grow(chararray);
	}

	//put at the end of str. test these fuckers! as in do unit_test.c that exercises all these functions

}

int main(int argc, char **argv)
{
	FILE *pInfile;
	FILE *pOutfile;

	char file_name = argv[1];

	pInfile = fopen(file_name, "r");
	pOutfile = fopen("out.data", "w");

	if (pInfile == NULL) {
		perror("Error while opening the file.\n");
		exit(-1);
	}

	struct stackvec_t* pHarbor = initstack();

	char c;
	char* token[] = malloc( sizeof( char ) * 20 ) ;
	while (	c = getc(pInfile) != EOF) {

		switch (c)
		{
		case '\n':
			printstack(pHarbor);
			freestack(pHarbor);
			pHarbor = initstack();
			break;
		case '\r':
			break;
		case ',':
			strappend( c, token );
			break;
		default:
			addtostack(token, pHarbor);
			break;
		}
	}


	//cleanup
	free(token);
	fclose( pInfile);
	fclose( pOutfile);
	freestack(pHarbor);

	return 0;
}
