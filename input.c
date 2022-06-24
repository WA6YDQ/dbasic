/* input.c
 *
 * part of dbasic
 * (C) k theis <theis.kurt@gmail.com> 2022
 *
 * BASIC INPUT statement
 *
 */

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
	extern FILE *fd[8];
	extern int fdnumber;
	int ISFILE=0;			// flag for file operations



	// debug 
	// printf("INPUT: %s\n",line);

	while (isdigit(*line)) line++;		// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces
	while (isalpha(*line)) line++;		// skip keyword
    if (isblank(*line)) while (isblank(*line)) line++;
    
	// test for file #
	if (*line == '#') {     // print to a file 
        ISFILE=1;
        line++;             // skip past #
        if (!(isdigit(*line))) {
            printf("Error - bad syntax in INPUT\n");
            return -1;
        }
		fdnumber = atoi(line);
        if (fdnumber < 1 || fdnumber > 7) {     // 1-7 allowed
            printf("Error - bad file number %d in INPUT\n",fdnumber);
            return -1;
        }
        /* make sure it's already opened */
        if (fd[fdnumber] == NULL) {
            printf("Error - file number %d is not opened\n",fdnumber);
            return -1;
        }
        line++;         // skip file number
    }

	/* format: 	input a					input #n a
	 * 			input a,b,c				input #n a,b,c
	 * 			input "enter a",a		input #n "enter a",a
	 * 			input a$				input #n a$
	 */

	while (1) {		// loop and test until EOL

		/* test for end of file - return error if reached */
		if (ISFILE && feof(fd[fdnumber])) {
			printf("Warning - Reached end of file\n");
			return 0;
		}

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
				if (!ISFILE) 					// don't show during file ops
					printf("%c",*line++);
			}
			line++;				// skip ending "
			continue;
		}
		
		/* if a$-z$ get string input */
		if (*line >= 'a' && *line <= 'z' && *(line+1) == '$') {
			char strvar = *line;		// get variable
			memset(CharVars[strvar-'a'],0,sizeof(CharVars[strvar-'a']));
			if (!ISFILE) printf("?");
			if (!ISFILE) fgets(getline,LINESIZE,stdin);
			if (ISFILE) {
				fgets(getline,LINESIZE,fd[fdnumber]);
				if (feof(fd[fdnumber])) {
					printf("Warning - reached end of file\n");
					return 0;
				}
			}
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
				char SUBNUM[LINESIZE]={'\0'};
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

			if (!ISFILE) printf("?");
			if (!ISFILE) fgets(getline,LINESIZE,stdin);		// 20 chars max for digits
			if (ISFILE) {
				fgets(getline,LINESIZE,fd[fdnumber]);
				if (feof(fd[fdnumber])) {
                    printf("Warning - reached end of file\n");
                    return 0;
                }
            }
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

