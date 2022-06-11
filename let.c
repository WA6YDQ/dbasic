/* LET (var) = (expression) */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "dbasic.h"

int run_let(char *line) {

	float eval(char *);
	extern float NumericVars[];
	extern char CharVars[][80];
	extern char *evalstring(char *);
	//extern char tempCharVar[LINESIZE];

	//printf("let:line=[%s]\n",line);

	while (isdigit(*line)) line++;  	// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces
	while (isalpha(*line)) line++;	// skip LET keyword

	/* format: 100 LET a=5, b=a*10+3, a$="this is a test"\n\0 */

	while (1) {
		if (*line == '\0') return 0;		// EOL
		if (*line == ',') {					// comma seperator
			line++;
			continue;
		}
		if (isblank(*line)) {
			line++;
			continue;
		}

		/* test strings: format LET a$="test", b$=a$ */
		if (*line >= 'a' && *line <= 'z' && *(line+1)== '$') {
			char strvar = *line; line+=2;		// skip past $
			if (isblank(*line)) while (isblank(*line)) line++;
			if (*line != '=') {
				printf("missing = in LET\n");
				return -1;
			}
			line++;			// skip =
			if (isblank(*line)) while (isblank(*line)) line++;
			
			/* see if =a$  */
			if (*line >='a' && *line <='z' && *(line+1)=='$') {
				strcpy(CharVars[strvar-'a'],CharVars[*line-'a']);
				line += 2;	// skip var
				continue;
			}
		
			/* everything between double quotes */
			if (*line == '"') {
				line++;		// skip "
				int n=0;
				while (*line != '"') {
					if (*line == '\0' || *line == '\n') {
						printf("Error - missing close quote in LET\n");
						return -1;
					}
					CharVars[strvar-'a'][n] = *line;
					n++; line++;
				}
				line++;	// skip final "
				continue;
			}

			/* test string functions: left$, mid$, right$, etc */
			char temp[LINESIZE]={}; int n=0;
			while (1) {
				if (*line == '\0' ) break;
				if (*line == ',' && *(line+1) == ' ') break;	// seperator btwn assignments
				temp[n] = *line;
				n++; line++;
				if (n > LINESIZE) {
					printf("Error - line too long\n");
					return -1;
				}
			}
			strcpy(CharVars[strvar-'a'],evalstring(temp));
			continue;

		}

		/* test numeric variable */
		if (*line >= 'a' && *line <= 'z') {
			char usevar = *line;
			char expr[LINESIZE]; memset(expr,0,LINESIZE); int n=0;
			line++;		// skip variable
			if (isblank(*line)) while (isblank(*line)) line++;
			if (*line != '=') {
				printf("Syntax Error in LET\n");
				return -1;
			}
			line++;		// got =  skip it
			while (1) {
				if (*line == ',' || *line == '\0' || *line == '\n') break;
				if (isblank(*line)) while (isblank(*line)) line++;
				expr[n] = *line;
				n++; line++;
				if (n>=LINESIZE) {
					printf("Error - expression too long\n");
					return -1;
				}
			}
			NumericVars[usevar - 'a'] = eval(expr);
			continue;
		}

		
		/* unknown char */
		printf("unknown character in LET statement [%c]\n",*line);
		line++;
		continue;
	}

	printf("Error - unknown error in LET\n");
	return -1;
}

