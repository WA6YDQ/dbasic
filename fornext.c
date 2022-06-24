/* fornext.c - run_for and run_next 
 *
 * part of dbasic
 *
 * (C) k theis <theis.kurt@gmail.com> 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int run_for(char *line) {
	extern char index_var;
	extern float index_start;
	extern float index_end;
	extern float index_step;
	extern float *NumVar[26];
	char expr[40] = {'\0'};
	int n=0;
	float eval(char *);

	// debug
	// printf("run_for: line [%s]\n",line);

	while (isdigit(*line)) line++;			// skip the line number
	while (isblank(*line)) line++;			// skip spaces
	while (isalpha(*line)) line++;			// skip FOR command
	if (isblank(*line)) while (isblank(*line)) line++;	// skip more spaces

	/* get index variable NOTE: will always be subscript 0 x(0)*/
	if (*line >= 'a' && *line <= 'z' && *(line+1) != '$' && *(line+1) != '(') 
		index_var = *line;
	else {
		printf("Error - undefined index variable %c\n",*line);
		return -1;
	}

	line++;				// skip index variable
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces

	/* test = */
	if (*line != '=') {
		printf("Error - missing = in for/next\n");
		return -1;
	} else
		line++;			// skip =

	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces

	/* get first index value */
	memset(expr,0,sizeof(expr)); n=0;
	while (1) {
		if (isblank(*line)) break;
		expr[n] = *line;
		n++; line++;
		if (n>=40) {
			printf("Error - expression too long\n");
			return -1;
		}
	}
	index_start = eval(expr);

	/* set up index variable */
	NumVar[index_var - 'a'][0] = index_start;

	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces

	/* test TO */
	if (*line == 't' && *(line+1) == 'o') {
		line += 2;
	} else {
		printf("Error - missing TO in for/next\n");
		return -1;
	}

	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces

	/* get second index variable */
	memset(expr,0,sizeof(expr)); n=0;
	while (1) {
		if (isblank(*line) || *line == '\n' || *line == '\0') break;
		expr[n] = *line;
		n++; line++;
		if (n>40) {
			printf("Error - expression too long\n");
			return -1;
		}
	}
	index_end = eval(expr);

	/* look for 'S' in STEP or \n for EOL */
	while (1) {
		line++;
		if (*line == '\n') break;
		if (*line == '\0') break;
		if (*line == 's') break;
	}

	// if no STEP command, set index_step to 1
	index_step = 1;

	if (*line == '\n' || *line == '\0') return 0;	// normal return, STEP not used
	if (*line=='s' && *(line+1)=='t' && *(line+2)=='e' && *(line+3)=='p') {
		line += 4;		// skip STEP keyword
	} else {
		printf("Error - unknown chars in FOR/NEXT statement\n");
		return -1;
	}

	// skip spaces, get step variable
	if (isblank(*line)) while (isblank(*line)) line++;
	memset(expr,0,sizeof(expr)); n=0;
	while (1) {
		if (*line == ' ' || *line == '\0' || *line == '\n') break;
		expr[n] = *line;
		if (n>40) {
			printf("Error - expression too long\n");
			return -1;
		}
		n++; line++;
	}
	index_step = eval(expr);

	/* test line end char */
	if (*line == '\n' || *line == '\0') return 0;
	printf("Error - excess characters in FOR/NEXT statement\n");
	return -1;
}





int run_next(char *line) {
	printf("run_next: line [%s]\n",line);
	return 0;
}

