/* def.c - define 26 user functions (DFA..DFZ) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int def(char *line) {

	/* format: 100 def fna = "2*x+sin(5)"
	 * This routine assigns the quoted formula to a 
	 * pre-defined string. When eval() encounters df{a..z}
	 * it evaluates the string and returns the result.
	 */

	extern char fn[26][80];		// function goes inside here
	//printf("def: line=[%s]\n",line);
	while (isdigit(*line)) line++;		// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;
	while (isalpha(*line)) line++;		// skip 'def'
	if (isblank(*line)) while (isblank(*line)) line++;

	if (*line=='f' || *(line+1)=='n') {
		char defchar = *(line+2);		// a..z
		if (defchar >= 'a' && defchar <= 'z') {
			int n=0; 
			line += 3;		// skip fna
			while (*line != '"') {
				if (*line == '\0' || *line == '\n') break;
				line++;
			}
			line++;		// point past initial "
			// did we break at EOL?
			if (*line == '\0' || *line == '\n') {
				printf("Error - missing function defination\n");
				return -1;
			}
			// get all between " " as the function rule
			//printf("def: loading the eqn\n");
			while (1) {
				if (*line == '\0' || *line == '\n') {
					printf("Error - missing closing quote in funct def\n");
					return -1;
				}
				if (*line == '"') break;
				fn[defchar-'a'][n] = *line;
				line++; n++;
				if (n>80) {
					printf("Error - definition too long\n");
					return -1;
				}
			}
			//printf("def: finished loading. n=%d\n",n);
			fn[defchar-'a'][n] = '\0';
			//printf("new def: fn%c=[%s]\n",defchar,fn[defchar-'a']);
			return 0;
		}
		printf("Error - bad function name\n");
		return -1;
	}
	return -1;
}

