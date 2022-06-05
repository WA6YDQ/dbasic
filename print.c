/* print BASIC command */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int run_print(char *line) {
	float result;					// result of expression
	char expr[40];
	float eval(char *);
	char linenumber[6];
	extern char CharVars[][80];
	sscanf(line,"%s",linenumber);
	//printf("PRINT: %s\n",line);
	
	while (isdigit(*line++));	// skip past line number
	if (isblank(*line)) while (isblank(*line)) line++;;	// skip spaces
	while (isalpha(*line++));	// skip 'print'
	
	while (1) {
		if (*line == '\0') break;
		
		if (isblank(*line)) {		// ignore blanks (space and tabs)
			line++;
			continue;
		}

		if (*line == '"') {			// print all between "
			line++;					// skip initial quote
			while (1) {
				printf("%c",*line);
				line++;
				if (*line == '\0') {	// unequal quotes
					printf("\nError UNEQUAL QUOTES in ");
					return -1;
				}
				if (*line != '"') continue;
				line++;				// skip closing "
				break;
			}
			continue;
		}

		if (*line == ',') {
			printf("   ");
			line++;
			continue;
		}

		if (*line == ';') {
			line++;
			continue;
		}

		/* strings a$ - z$ */
		if (*line >= 'a' && *line <= 'z' && *(line+1) == '$') {
			printf("%s",CharVars[*line-'a']);
			line +=2;
			continue;
		}


		/* vars: examples: print a+5, (b*72/c), r/(10+6)/w; */
		if (*line == '(' || *line == '-' || (*line >= 'a' && *line <= 'z')) {		// expression starting with a var
			int n=0;
			memset(expr,0,sizeof(expr));
			while (1) {				// put complete expression in line
				if (*line == '\0' || *line == ',' || *line == ';') break;
				expr[n++] = *line++;
			}
			//printf("sending to eval:{%s} ",expr);
			result = eval(expr);
			printf("%g",result);
			/*
			if (result > (int)result)  
				printf("%f",result);
			else
				printf("%8.0f",result);
			*/
			continue;
		}

		/* numbers: examples: print 10, 35+7, 5*(a+3) */
		if (*line == '(' || *line == '-' || (*line >= '0' && *line <= '9')) {		// expression starting with a number
			int n=0;
			memset(expr,0,sizeof(expr));
			while (1) {			// put complete expression in line
				if (*line == '\0' || *line == ',' || *line == ';') break;
				expr[n++] = *line++;
			}
			//printf("sending to eval:{%s} ",expr);
			result = eval(expr);
			printf("%g",result);
			/*
			if (result > (int)result)
				printf("%f",result);
			else
				printf("%8.0f",result);
			*/
			continue;
		}

		printf("Error - unknown char in PRINT %c\n",*line);
		return -1;
		//line++;
		//continue;
	}
	if (*(line-1) != ';') printf("\n");
	return 0;
}
