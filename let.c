/* LET (var) = (expression) */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


int run_let(char *line) {

	int eval(char *);
	extern int NumericVars[];
	extern char CharVars[][80];

	//printf("let:line=[%s]\n",line);

	while (isdigit(*line)) line++;  	// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces
	while (isalpha(*line)) line++;	// skip LET
	while (!isalpha(*line)) line++;	// look for variable

	/* test strings */
	if (*line >= 'a' && *(line+1) == '$' && *line <= 'z') {
		char strvar = *line; line+=2;		// skip past $
		if (*line == ' ') while (isblank(*line)) line++;
		if (*line != '=') {
			printf("missing = in LET\n");
			return -1;
		}
		line++;			// skip =
		if (*line == ' ') while (isblank(*line)) line++;
		if (*line != '"') {
			printf("Bad format in LET for a string\n");
			return -1;
		}
		line++;			// skip "
		int n=0;
		memset(CharVars[strvar-'a'],0,sizeof(CharVars[strvar-'a']));
		while (1) {
			if (*line == '\0' || *line == '\n') {
				printf("Error in LET - missing end quote\n");
				return -1;
			}
			if (*line == '"') break;
			CharVars[strvar-'a'][n] = *line;
			n++; line++;
		} // last char is \0 
		return 0;
	}
		



	/* test numeric */
	if (*line >= 'a' && *line <= 'z') {		// numeric assignment
		char usevar = *line;				// get the variable to save to
		//printf("saving to var [%c]\n",usevar);
		while (*line != '=') {				// find =
			line++;
			if (*line == '\0') {
				printf("Missing assignment\n");
				return 1;
			}
		}
		//printf("%c should be =\n",*line);
		line++;								// skip =
		if (isblank(*line)) while (isblank(*line)) line++;

		int res = eval(line);
		//printf("eval return is [%d]\n",res);
		NumericVars[usevar-'a'] = res;

		return 0;
	}

	printf("var is not numeric\n");
	/* test for strings here */
	return 0;
}

