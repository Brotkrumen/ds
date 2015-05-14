#include <stdio.h>

typedef unsigned int uint;

int main ( int argc, char **argv )
{
        FILE *pInfile;
	FILE *pOutfile;

	char ch, file_name[255];	

	pInfile = fopen(file_name,"r");
	pOutfile = fopen( "out.data","w");

	if( fp == NULL ) {
	      perror("Error while opening the file.\n");
	      exit(EXIT_FAILURE);
	   }

	return 0;
}
