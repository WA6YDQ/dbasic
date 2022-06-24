/* print.c
 *
 * part of dbasic
 * (C) k theis <theis.kurt@gmail.com>
 *
 * BASIC PRINT statement
 *
 */

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
				if (!ISFILE) printf("%c",*line);
				if (ISFILE)	fprintf(fd[fdnumber],"%c",*line);
				line++;
			}
			line++;		// skip closing "
			continue;
		}

		/* comma shows 3 spaces */
		if (*line == ',') {
			if (!ISFILE) printf("   ");
			if (ISFILE)	fprintf(fd[fdnumber],"   ");
			line++;
			continue;
		}

		/* SPC() function */
		if (*line == 's' && *(line+1)=='p' && *(line+2)=='c' && *(line+3)=='(') {
			line += 4;
			char TEMP[LINESIZE]={'\0'}; int n=0; int pc=0;
			while (1) {
				if (*line == ')' && pc == 0) break;
				if (*line == '(') pc++;
				if (*line == ')') pc--;
				if (*line == '\0') {
					printf("Error - syntax error in SPC()\n");
					return -1;
				}
				TEMP[n] = *line;
				n++; line++;
			}
			line++;		// skip )
			n = eval(TEMP);
			for (int i=0; i<n; i++) {
				if (!ISFILE) printf(" ");
				if (ISFILE) fprintf(fd[fdnumber]," ");
			}
			continue;
		}


		/* strings a$ - z$ */
		if (*line >= 'a' && *line <= 'z' && *(line+1) == '$') {
			if (!ISFILE) printf("%s",CharVars[*line-'a']);
			if (ISFILE)	fprintf(fd[fdnumber],"%s",CharVars[*line-'a']);
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
		if (!ISFILE) printf("%g",res);
		if (ISFILE)	fprintf(fd[fdnumber],"%g",res);
		continue;


	}

	if (*(line-1) == ';') return 0;
	if (!ISFILE) printf("\n");
	if (ISFILE)	fprintf(fd[fdnumber],"\n");
	return 0;
}
