#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//should be unsigned 32bit int
typedef unsigned long int uint;

const uint INITIAL_STACK_SIZE = 50;
const uint INITIAL_HARBOR_SIZE = 10;
const double GROWTH_FACTOR = 1.2;

/******************************************************************************
* Description	bookkeeping struct for one container stack.
*
* members:
*	length		length of the allocated array
*	last		last valid entry in the array
*	array*		pointer to first element
*				holds uint values
*
******************************************************************************/

typedef struct stack_t {
	uint length;
	uint last;
	uint* array;
} containerStack_t;


/******************************************************************************
* Description	sets initial values of containerstack and handles mem alloc
*
* Param			length of array to be allocated
*
* Returns		pointer to stack created
*
******************************************************************************/

containerStack_t* initContainerstack(uint maxlen) {
	//ptr to stack
	containerStack_t* pStack = (containerStack_t*)malloc(sizeof(containerStack_t));
	if (pStack == NULL) { fputs("Memory error", stderr); exit(2); }

	//set array of container length and allocate mem
	pStack->length = maxlen;
	uint* array = (uint*)malloc(sizeof(uint) *pStack->length);
	if (array == NULL) { fputs("Memory error", stderr); exit(2); }

	//assign pointer of the array to the stack
	pStack->array = array;

	//set first valid entry position
	pStack->last = 0;

	return pStack;
}

/******************************************************************************
* Description	frees the allocated memory of the containerStack_t
*				calls free on the array and then on the struct
*
* param			pointer to stack
*
******************************************************************************/

void freestack(containerStack_t* pStack) {

	free(pStack->array);
	free(pStack);

}

/******************************************************************************
* Description	grows the size of a containerstack by GROWTH_FACTOR
*				copies values, not ptrs, to the new stack.
*				old stack gets freed
*
* param			takes address of a pointer to be changed
*
******************************************************************************/

void growStack(containerStack_t ** ptr_pStack) {

	//deref ptr to ptr to have the pointer to work with
	containerStack_t * pStack = *ptr_pStack;

	containerStack_t* newStack = initContainerstack((int)(pStack->length * GROWTH_FACTOR));

	for (uint i = 0; i <= pStack->last; i++) {
		newStack->array[i] = pStack->array[i];
	}

	newStack->last = pStack->last;

	freestack(pStack);

	*ptr_pStack = newStack;
}

/******************************************************************************
* Description	bookkeeping struct for the "harbor".
*				array of stack_t structs. equivalent to how many stacks
*				of containers exist
*
* members:
*	length		length of the allocated array of stack_t*
*	last		last valid entry in the array
*	array*		pointer to first element
*				holds containerStack_t pointers
*
******************************************************************************/

typedef struct stackvec_t {
	uint length;
	uint last;
	int isEmpty;
	containerStack_t** array;
} harbor_t;


/******************************************************************************
* Description	initializes the harbor_t, the harbor.
*				allocates a stack to calculate the size of the necessary alloc
*				to avoid having to do that manually
*				its a bold move cotton, lets see if it works out.
*
*
* returns		pointer to harbor_t
*
******************************************************************************/

harbor_t* initHarbor( uint maxlen ) {

	//ptr to a harbor struct
	harbor_t* pHarbor = (harbor_t *)malloc(sizeof(harbor_t));
	if (pHarbor == NULL) { fputs("Memory error", stderr); exit(2); }

	pHarbor->last = 0;
	pHarbor->isEmpty = 1;
	pHarbor->length = maxlen;
	pHarbor->array = (containerStack_t**)malloc(sizeof(containerStack_t*) * pHarbor->length);
	if (pHarbor->array == NULL) { fputs("Memory error", stderr); exit(2); }

	return pHarbor;
}

/******************************************************************************
* Description	grows the size of a harbor by GROWTH_FACTOR
*				copies pointer so the old stacks to new array
*				old array gets freed and the stacks hopefully stay intact
*
* param			pointer to Harbor
*
******************************************************************************/

void growHarbor(harbor_t ** ptr_pHarbor) {

	harbor_t* pHarbor = *ptr_pHarbor;

	harbor_t* newHarbor = initHarbor((int)(pHarbor->length * GROWTH_FACTOR));

	for (uint i = 0; i <= pHarbor->last; i++) {
		newHarbor->array[i] = pHarbor->array[i];
	}

	newHarbor->last = pHarbor->last;
	newHarbor->isEmpty = 0;
	
	free(pHarbor->array);
	free(pHarbor);

	*ptr_pHarbor = newHarbor;

}


/******************************************************************************
* Description	prints the stackvec_t, the harbor
*
* Format:		$stack_count $space $stack $space $stack ...
*
* param			pointer to "harbor"
*
******************************************************************************/

void printstack( harbor_t* pHarbor) {

	if (pHarbor->isEmpty == 1) {
		return;
	}

	printf( "%i ", (pHarbor->last)+1 );

	containerStack_t* pStack;
	for (uint itHarbor = 0; itHarbor <= pHarbor->last; itHarbor++) {
		pStack = pHarbor->array[itHarbor];
		for (uint itStack = 0; itStack <= pStack->last; itStack++) {
			
			if (itStack != 0) {
				printf("%s", ",");
			}

			printf("%i", pStack->array[itStack]);
		}

		printf("%s"," ");

	}

	printf("%s","\n");
}

/******************************************************************************
* Description	frees the allocated memory of the harbor_t or "harbor"
*				goes through all array members and calls free on them and then
*				on the struct
*
* param			pointer to "harbor"
*
******************************************************************************/

void freeHarbor(harbor_t* pHarbour) {

	for (uint i = 0; i <= pHarbour->last && pHarbour->isEmpty != 1; i++) {
		freestack(pHarbour->array[i]);
	}

	free(pHarbour->array);
	free(pHarbour);

}

/******************************************************************************
* Description	inserts the number "container" into the appropriate stack
*				if the harbor is overflowing, calls grow()
*				if the stack the newContainer is to be placed on is overflowing,
*				calls grow on that.
*				
* Algorithm:	1)	check each stack if the newContainer is either equal to the
*					one on top of the stack
*						if yes, put it on it and next. best case
*					check if this stack can hold the container and is the best
*					stack to do that so far
*						if yes, save the stack position in the harbor*				
*
*				3)	if one of the two cases in 2) have been found, put the
*					newContainer on it
*					if not, make a new stack
*
*				4)	if no new containers are coming, aka newline, prints and
*					discards harbor. makes a new one until there are no new
*					harbors to make
*
* returns		pointer to harbor_t
*
******************************************************************************/

void addtostack(uint newContainer, harbor_t** ptr_pHarbor) {

	harbor_t * pHarbor = *ptr_pHarbor;

	uint lastContainerOnThisStack = 0;
	uint distanceToBeststack = 0 - 1;
	uint bestStack = 0;
	int foundstack = 0;

	for (uint itHarbor = 0; itHarbor <= pHarbor->last && pHarbor->isEmpty != 1; itHarbor++) {

		lastContainerOnThisStack = pHarbor->array[itHarbor]->array[pHarbor->array[itHarbor]->last];

		//if container on current stack has same value as the new container, 
		//best case, drop here
		if (lastContainerOnThisStack == newContainer) {
			bestStack = itHarbor;
			foundstack = 1;
			break;
		}

		if (	lastContainerOnThisStack > newContainer 
			&&	distanceToBeststack > (lastContainerOnThisStack-newContainer) ) {
			distanceToBeststack = lastContainerOnThisStack - newContainer;
			bestStack = itHarbor;
			foundstack = 1;
		}

	}

	if (pHarbor->last == (pHarbor->length - 1 ) ) {
		growHarbor(&pHarbor);
		*ptr_pHarbor = pHarbor;
	}


	if (foundstack == 1) {
		containerStack_t* pStack = pHarbor->array[bestStack];
		if (pStack->last == ( pStack->length - 1 ) ) {

			//need to pass the address of the pointer stored in pHarbor
			//to change what that pointer points to, not just pStack
			growStack(&(pHarbor->array[bestStack]));

		}
		else {
			pStack->last++;
			pStack->array[pStack->last] = newContainer;
		}
	} else {
		containerStack_t* pStack = initContainerstack( INITIAL_STACK_SIZE );
		pStack->array[pStack->last] = newContainer;

		if (pHarbor->isEmpty != 1 ) {
			pHarbor->last++;
		} else {
			pHarbor->isEmpty = 0;
		}

		pHarbor->array[pHarbor->last] = pStack;
	}
}


/******************************************************************************
* Description	source: cpp reference for fread
*				points given char* to place in mem where fileconts was put
*
* param			address of pointer to be repointed
*				char * to filename
*
* returns		length of the filebuffer
*
******************************************************************************/
uint readfiletobuffer( char ** ptr_buffer, char* file_name ) {

	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;

	pFile = fopen(file_name, "rb");
	if (pFile == NULL) { fputs("File error", stderr); exit(1); }

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	// +1 to allocate the last \n at
	buffer = (char*)malloc((sizeof(char)*lSize));
	if (buffer == NULL) { fputs("Memory error", stderr); exit(2); }

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) { fputs("Reading error", stderr); exit(3); }

	/* the whole file is now loaded in the memory buffer. */

	// terminate
	fclose(pFile);
	
	*ptr_buffer = buffer;

	return result;
}

void appendtofile( harbor_t* pHarbor, char* path) {

	if (pHarbor->isEmpty == 1) {
		return;
	}

	FILE *pOutfile = fopen( path , "a");
	if (pOutfile == NULL) 	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf( pOutfile, "%i ", (pHarbor->last) + 1);

	containerStack_t* pStack;
	for (uint itHarbor = 0; itHarbor <= pHarbor->last; itHarbor++) {
		pStack = pHarbor->array[itHarbor];
		for (uint itStack = 0; itStack <= pStack->last; itStack++) {

			if (itStack != 0) {
				fprintf(pOutfile, "%s", ",");
			}

			fprintf(pOutfile, "%i", pStack->array[itStack]);
		}

		fprintf(pOutfile, "%s", " ");

	}

	fprintf(pOutfile, "%s", "\n");

	fclose(pOutfile);

}

void clearfile( char* path ) {
	FILE *pOutfile = fopen(path, "w");
	if (pOutfile == NULL) 	{
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(pOutfile, "%s","");

	fclose(pOutfile);
}

/******************************************************************************
* Description Validates input file data
*
* param		char* to filecontents
			int length of buffer
*
******************************************************************************/

void validateInput(char* filebuffer, int bufferlen) {

	int linecounter = 1;
	int poscounter = 0;
	int founderror = 0;
	int requireDigit = 0;
	char* endOfBuffer = filebuffer + bufferlen * sizeof(char) - 1;	//pos of one past last valid cahr

	//endOfBuf - (int)filebuffer should be the length of the filebuffer
	for (int i = 0; i <= endOfBuffer - filebuffer; i++) {
		
		if (filebuffer[i] != '\n') {
			poscounter++;
		}
		else {
			poscounter = 0;
			linecounter++;
		}

		if ( (!isdigit(filebuffer[i]) && filebuffer[i] != ',' && filebuffer[i] != '\n' )
			|| (!isdigit(filebuffer[i]) && requireDigit )) {

			if (filebuffer[i] == '\n') {
				printf("char '\\n' with val %i in line %i at pos %i is not a valid input.\n",
					(int)filebuffer[i], linecounter, poscounter);
			} else {
				printf("char '%c' with val %i in line %i at pos %i is not a valid input.\n",
					filebuffer[i], (int)filebuffer[i], linecounter, poscounter);
			}
			
			founderror = 1;
		}

		//to get the ",\n" case
		if (filebuffer[i] == ',') {
			requireDigit = 1;
		} else {
			requireDigit = 0;
		}
	}

	if (founderror) {
		printf("\n%s\n", "only \\n separated lines with comma separated positive integers allowed");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	char* file_name = argv[1];

	if (file_name == NULL) {
		printf("No file specified. Usage: main /path/to/file");
		exit(-1);		
		//file_name = "C:/workspace/ds/harborProblem/testdata/input3.data";
	}

	char* filebuffer; 
	
	//points the in char* at contents of file read
	uint bufferlen = readfiletobuffer( &filebuffer, file_name);
	
	char* endOfBuff = filebuffer + bufferlen * sizeof(char);
	char* pEnd;

	validateInput(filebuffer, bufferlen);

	uint thisnum;
	harbor_t* pHarbor;

	//clearfile("out.data");

	pHarbor = initHarbor(INITIAL_HARBOR_SIZE);

	//-1 compensate for strtoul +1
	pEnd = filebuffer -1 ;
	while (1) {

		thisnum = strtoul(pEnd + 1, &pEnd, 10);

		//48 is ascii 0. check to distinguish between (null) value return
		//since value 0 is in the input data
		if (thisnum == 0 && (*(pEnd - 1) != 48 )) {
			printf("%s\nnumber:%i pEnd addr%i endOfBuff addr: %i\n"
			, "couldnt read number before eof encountered. exit"
			, *(pEnd-1), pEnd, endOfBuff );
			exit(-1);
		}

		addtostack(thisnum, &pHarbor);

		if (*pEnd == '\n') {
			printstack(pHarbor);
			//appendtofile(pHarbor, "out.data");
			freeHarbor(pHarbor);
			pHarbor = initHarbor(INITIAL_HARBOR_SIZE);
		}
		
		//last char in input data is always linebreak. dont wanna rely on that
		if (pEnd == endOfBuff-1 || pEnd == endOfBuff) {
			printstack(pHarbor);
			//appendtofile(pHarbor, "out.data");
			freeHarbor(pHarbor);
			break;
		}
	}

	//cleanup
	free(filebuffer);
	return 0;
}
