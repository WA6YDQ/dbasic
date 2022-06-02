/* print BASIC command */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int run_print(char *line) {
	int result;					// result of expression
	char expr[40];
	int eval(char *);
	char linenumber[6];
	extern char CharVars[][80];
	extern int NumericVars[];
	sscanf(line,"%s",linenumber);
	//printf("PRINT: %s\n",line);
	
	while (isdigit(*line++));	// skip past line number
	while (isspace(*line++));	// skip spaces
	while (isalpha(*line++));	// skip 'print'
	
	while (1) {
		if (*line == '\0') break;
		
		if (*line == ' ') {
			printf(" ");
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


		/* print a+5, (b*72/c), r/(10+6)/w; */
		if (*line == '(' || *line == '-' || (*line >= 'a' && *line <= 'z')) {		// expression starting with a var
			int n=0;
			memset(expr,0,sizeof(expr));
			while (1) {				// put complete expression in line
				if (*line == '\0' || *line == ',' || *line == ';') break;
				expr[n++] = *line++;
			}
			//printf("sending to eval:{%s} ",expr);
			result = eval(expr);
			printf("%d",result);
			continue;
		}

		/* print 10, 35+7, 5*(a+3) */
		if (*line == '(' || *line == '-' || (*line >= '0' && *line <= '9')) {		// expression starting with a number
			int n=0;
			memset(expr,0,sizeof(expr));
			while (1) {			// put complete expression in line
				if (*line == '\0' || *line == ',' || *line == ';') break;
				expr[n++] = *line++;
			}
			//printf("sending to eval:{%s} ",expr);
			result = eval(expr);
			printf("%d",result);
			continue;
		}

		printf("unknown char[%c] in line number %s\n",*line,linenumber);
		line++;
		continue;
	}
	if (*(line-1) != ';') printf("\n");
	return 0;
}
