/* def.c - define 26 user functions (DFA..DFZ) 
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
#include "dbasic.h"

int def(char *line) {

	/* format: 100 def fna 1/(2*3.14156*sqr(l*c))
	 * This routine assigns the quoted formula to a 
	 * pre-defined string. When eval() encounters df{a..z}
	 * it evaluates the string and returns the result.
	 */

	extern char fn[26][LINESIZE];		// function goes inside here
	while (isdigit(*line)) line++;		// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;
	while (isalpha(*line)) line++;		// skip 'def'
	if (isblank(*line)) while (isblank(*line)) line++;

	if (*line=='f' || *(line+1)=='n') {
		char defchar = *(line+2);		// a..z
		if (defchar >= 'a' && defchar <= 'z') {
			int n=0; 
			line += 3;		// skip 3 chars of fnx
			// get all up to \0 or \n, save to fnx string
			while (1) {
				if (*line == '\0' || *line == '\n') break;
				if (*line == '"') {			// ignore ""
					line++;
					continue;
				}
				fn[defchar-'a'][n] = *line;
				line++; n++;
				if (n>sizeof(fn[defchar-'a'])) {
					printf("Error - function is too long\n");
					return -1;
				}
			}
			if (n==0) {
				printf("Error - missing function definition");
				return -1;
			}

			return 0;		// normal end

		}
		printf("Error - bad function name\n");
		return -1;
	}
	return -1;
}

