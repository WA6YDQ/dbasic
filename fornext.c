/* fornext.c - run_for and run_next */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int run_for(char *line) {
	extern char index_var;
	extern int index_start;
	extern int index_end;
	extern int index_step;
	extern int NumericVars[];
	char expr[40] = {};
	int n=0;
	int eval(char *);

	//printf("run_for: line [%s]\n",line);
	while (isdigit(*line)) line++;			// skip the line number
	while (isblank(*line)) line++;			// skip spaces
	while (isalpha(*line)) line++;			// skip FOR command
	if (isblank(*line)) while (isblank(*line)) line++;	// skip more spaces

	/* get index variable */
	if (*line >= 'a' && *line <= 'z') 
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
	//printf("run_for: index_start = %d\n",index_start);

	/* set up index variable */
	NumericVars[index_var - 'a'] = index_start;
	//printf("run_for: %c set to %d\n",index_var,NumericVars[index_var - 'a']);

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
	//printf("run_for: index_end = %d\n",index_end);

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
		//printf("run_for: found step\n");
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
	//printf("run_for: index_step = %d\n",index_step);

	/* test line end char */
	if (*line == '\n' || *line == '\0') return 0;
	printf("Error - excess characters in FOR/NEXT statement\n");
	return -1;
}





int run_next(char *line) {
	printf("run_next: line [%s]\n",line);
	return 0;
}

