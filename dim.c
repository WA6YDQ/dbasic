/* dim.c - dbasic numeric arrays 
 *
 * part of dbasic
 *
 * (C) k theis <theis.kurt@gmail.com> 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define _ISOC99_SOURCE
#include <math.h>
#include "dbasic.h"
#include <string.h>

int dim(char *line) {

	extern float *NumBase[26];
	extern float *NumVar[26];
	extern int NumSize[26];
	float eval(char *);
	int memsize=0;		// calculated array size for later
	int dimsize=0;		// user-supplied array size
	char STRING[LINESIZE];		// holds user supplied array size
	int n=0;

	// format 100 dim a(1000)
	
	while (isdigit(*line)) line++;						// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces
	if (isalpha(*line)) while (isalpha(*line)) line++;	// skip 'dim'
	
	while (1) {
	
		if (isblank(*line)) while (isblank(*line)) line++;  // skip spaces
		if (*line == '\0' || *line == '\n') return 0;
		if (*line == ',') {
			line++;			// skip commas
			continue;
		}

		// should be pointing to variable to be re-dimmed
		if (isalpha(*line)) {
			void *temp;
			char charvar = *line;
			line++;				// skip variable
			if (*line != '(') {
				printf("Error - Syntax Error in dim statement\n");
				return -1;
			}
			line++;		// skip (
			// get the numeric value of all between ()
			memset(STRING,0,LINESIZE);
			n=0;
			while (1) {
				if (*line == ')') break;
				STRING[n] = *line;
				n++; line++;
				if (n > LINESIZE) {
					printf("Error - line too long in dim statement\n");
					return -1;
				}
			}
			dimsize = eval(STRING);
			if (dimsize <= NumSize[charvar-'a']) {		// trying to make array smaller
				printf("Error - attempting to decrease array size\n");
				return -1;
			}
			NumSize[charvar-'a'] = dimsize;		// store change
	
			memsize = dimsize*sizeof(float);
			// see which variable to re-dim
				
			temp = realloc(NumBase[charvar-'a'],memsize);
			if (temp == NULL) {
				printf("Error - out of memory\n");
				return -1;
				}
			NumBase[charvar-'a'] = temp;
			NumVar[charvar-'a'] = NumBase[charvar-'a'];
	
			// dim statement executed
			continue;
			
		}
	line++;		// get next char
	}

	printf("Error - bad variable in dim statement\n");
	return -1;
}


