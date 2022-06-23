/* print BASIC command */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#define _ISOC99_SOURCE
#include <math.h>
#include "dbasic.h"


int run_print(char *line) {
	char expr[LINESIZE];
	int n=0;
	float eval(char *);
	extern char CharVars[][80];
	extern char *evalstring(char *);
	//extern char tempCharVar[MAXLINESIZE];
	//extern float *NumVar[26];
	extern int error;
	error = 0;
	extern FILE *fd[8];
	extern int fdnumber;
	int ISFILE=0;			// flag for file operations

	while (isdigit(*line++));							// skip past line number
	if (isblank(*line)) while (isblank(*line)) line++;;	// skip spaces
	while (isalpha(*line++));							// skip 'print'
	
	// test for file#
	if (isblank(*line)) while (isblank(*line)) line++;
	if (*line == '#') {		// print to a file 
		ISFILE=1;
		line++;				// skip past #
		if (!(isdigit(*line))) {
			printf("Error - bad syntax in PRINT\n");
			return -1;
		}
		fdnumber = atoi(line);
		if (fdnumber < 1 || fdnumber > 7) {		// 1-7 allowed
			printf("Error - bad file number %d in PRINT\n",fdnumber);
			return -1;
		}
		/* make sure it's already opened */
		if (fd[fdnumber] == NULL) {
			printf("Error - file number %d is not opened\n",fdnumber);
			return -1;
		}
		line++;			// skip file number
	}



	while (1) {
		if (*line == '\0') break;
		
		if (isblank(*line) || *line==';') {		// ignore blanks (space and tabs) and ;
			line++;
			continue;
		}

		/* print all between double quotes */
		if (*line == '"') {
			line++;					// skip initial quote
			while (1) {
				if (*line == '"') break;
				if (*line == '\0') {	// unequal quotes
					printf("\nError - unequal quotes in PRINT. \n");
					return -1;
				}
				if (!ISFILE) 
					printf("%c",*line);
				else
					fprintf(fd[fdnumber],"%c",*line);
				line++;
			}
			line++;		// skip closing "
			continue;
		}

		/* comma shows 3 spaces */
		if (*line == ',') {
			if (!ISFILE) 
				printf("   ");
			else
				fprintf(fd[fdnumber],"   ");
			line++;
			continue;
		}

		/* strings a$ - z$ */
		if (*line >= 'a' && *line <= 'z' && *(line+1) == '$') {
			if (!ISFILE) 
				printf("%s",CharVars[*line-'a']);
			else
				fprintf(fd[fdnumber],"%s",CharVars[*line-'a']);
			line +=2;
			continue;
		}

		// anything else is an expression
		memset(expr,0,sizeof(expr)); n=0;
		float res=0;
		while (1) {
			if (*line==',' || *line==';' || *line=='\0' || *line=='\n') break;
			expr[n] = *line;
			n++; line++;
		}
		/* test numeric expression */
		res = eval(expr);
		/* SPC() returns NAN - ignore this */
		if (strncmp(expr,"spc(",4)==0 && isnan(res)) continue;
		if (error == -1) {
			printf("\nError - eval error in print()\n");
			return -1;
		}
		if (!ISFILE)
			printf("%g",res);
		else
			fprintf(fd[fdnumber],"%g",res);
		continue;


	}

	if (*(line-1) == ';') return 0;
	if (!ISFILE)
		printf("\n");
	else
		fprintf(fd[fdnumber],"\n");
	return 0;
}
