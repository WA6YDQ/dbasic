/* input.c - run INPUT command */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int run_input(char *line) {
	extern int NumericVars[];
	extern char CharVars[][80];
	char getline[20];
	
	//printf("INPUT: %s\n",line);

	while (isdigit(*line)) line++;		// skip line number
	if (isspace(*line)) while (isspace(*line)) line++;	// skip spaces
	while (isalpha(*line)) line++;		// skip keyword
	if (isspace(*line)) while (isspace(*line)) line++;	// skip spaces

	/* format: 	input a
	 * 			input a,b,c
	 * 			input "enter a",a
	 */

	while (1) {		// loop and test until EOL

		if (*line == '\n' || *line == '\0') break;	// EOL
		if (*line == ',' || *line == ';' || *line == ' ') {			// ignore ,;[ ]
			line++;
			continue;
		}
		
		/* print all between " "  */
		if (*line == '"') {
			line++;				// skip initial "
			while (1) {
				if (*line == '"') break;
				printf("%c",*line++);
			}
			line++;				// skip ending "
			continue;
		}
		
		/* if a$-z$ get string input */
		if (*line >= 'a' && *line <= 'z' && *(line+1) == '$') {
			char strvar = *line;		// get variable
			//printf("input: var is %c\n",strvar);
			memset(CharVars[strvar-'a'],0,sizeof(CharVars[strvar-'a']));
			fgets(getline,80,stdin);
			getline[strlen(getline)-1] = '\0';		// strip off \n
			strcpy(CharVars[strvar-'a'],getline);
			line += 2;
			continue;
		}
			


		/* if a-z get numeric input */
		if (*line >= 'a' && *line <= 'z') {
			fgets(getline,20,stdin);
			NumericVars[*line-'a'] = atoi(getline);
			line++;
			continue;
		}

		/* unknown character */
		printf("Error - unknown character %c \n",*line);
		return -1;
	}

	return 0;
}

