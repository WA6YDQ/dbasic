/* print BASIC command */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "dbasic.h"

int run_print(char *line) {
	char expr[LINESIZE];
	int n=0;
	float eval(char *);
	//char linenumber[6];
	extern char CharVars[][80];
	//sscanf(line,"%s",linenumber);
	//printf("PRINT: %s\n",line);
	
	while (isdigit(*line++));							// skip past line number
	if (isblank(*line)) while (isblank(*line)) line++;;	// skip spaces
	while (isalpha(*line++));							// skip 'print'
	
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
				printf("%c",*line);
				line++;
			}
			line++;		// skip closing "
			continue;
		}

		/* comma shows 3 spaces */
		if (*line == ',') {
			printf("   ");
			line++;
			continue;
		}

		/* strings a$ - z$ */
		if (*line >= 'a' && *line <= 'z' && *(line+1) == '$') {
			printf("%s",CharVars[*line-'a']);
			line +=2;
			continue;
		}

		// anything else is an expression
		memset(expr,0,sizeof(expr)); n=0;
		while (1) {
			if (*line==',' || *line==';' || *line=='\0' || *line=='\n') break;
			expr[n] = *line;
			n++; line++;
		}
		printf("%g",eval(expr));
		continue;


	}

	if (*(line-1) == ';') return 0;
	printf("\n");
	return 0;
}
