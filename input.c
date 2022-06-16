/* input.c - run INPUT command */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dbasic.h"

int run_input(char *line) {
	extern float *NumVar[26];
	extern char CharVars[][LINESIZE];
	char getline[LINESIZE];
	float eval(char *);
	extern int NumSize[26];

	// debug 
	// printf("INPUT: %s\n",line);

	while (isdigit(*line)) line++;		// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces
	while (isalpha(*line)) line++;		// skip keyword
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces

	/* format: 	input a
	 * 			input a,b,c
	 * 			input "enter a",a
	 */

	while (1) {		// loop and test until EOL

		//if (*line == '\n' || *line == '\0') break;	// EOL
		if (*line == '\0') break;	// EOL
		if (*line == ',' || *line == ';' || *line == ' ') {			// ignore ,;[ ]
			line++;
			continue;
		}
		
		/* print all between " "  */
		if (*line == '"') {
			line++;				// skip initial "
			while (1) {
				if (*line == '\0' || *line == '\n') {
					printf("Error - missing double quote\n");
					return -1;
				}
				if (*line == '"') break;
				printf("%c",*line++);
			}
			line++;				// skip ending "
			continue;
		}
		
		/* if a$-z$ get string input */
		if (*line >= 'a' && *line <= 'z' && *(line+1) == '$') {
			char strvar = *line;		// get variable
			memset(CharVars[strvar-'a'],0,sizeof(CharVars[strvar-'a']));
			printf("?");
			fgets(getline,LINESIZE,stdin);
			getline[strlen(getline)-1] = '\0';		// strip off \n
			strcpy(CharVars[strvar-'a'],getline);
			line += 2;
			continue;
		}
			


		/* if a-z get numeric input */
		if (*line >= 'a' && *line <= 'z' && *(line+1) != '$') {
			int subscript = 0;
			char charvar = *line;
			line++;
			if (*line == '(') {	// using subscripts
				line++;
				char SUBNUM[LINESIZE]={};
				int n=0;
				while (1) {
					if (*line == ')') break;
					SUBNUM[n] = *line;
					line++; n++;
				}
				subscript = eval(SUBNUM);
				line++;
			}
			// error bounds checking
            if (subscript < 0 || subscript > NumSize[charvar-'a']) {
                printf("Error - bounds error in subscript: %c(%d)\n",charvar,subscript);
                return -1;
            }

			printf("?");
			fgets(getline,20,stdin);		// 20 chars max for digits
			NumVar[charvar-'a'][subscript] = eval(getline);		// save value
			line++;
			continue;
		}

		/* unknown character */
		printf("Error - unknown character %c \n",*line);
		return -1;
	}

	return 0;
}

