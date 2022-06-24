/* let.c 
 *
 * part of dbasic 
 * (C) k theis <theis.kurt@gmail.com> 2022
 *
 * parse the let command, assign variables
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "dbasic.h"

int let(char *line) {
	char charvar = '\0';		// character variable
	int subscript = 0;			// array substring
	char tmpstr[80] = {'\0'};		// used to extract vals
	int n=0;
	extern int error;
	float res = 0;
	extern float *NumVar[26];	// numeric varaibles to use
	extern char CharVars[][80];	// character variables to use
	extern int NumSize[26];		// holds subscript size
	float eval(char *);			// number evaluation function
	char *evalstring(char *);	// string eval function

	/* examples: 
	 *
	 * 10 let a=100, b=sin(30*12), c=a+b*c, a(2)=3, a(n+4)=(n*n) 
	 * 10 let a$="abc", b$=left$(a$,1), c$=a$+"abc"
	 *
	 */

	/* skip spaces, line number, let statement */
	if (isdigit(*line)) while (isdigit(*line)) line++;
	if (isblank(*line)) while (isblank(*line)) line++;
	while (!(isblank(*line))) line++;

letloop:
	/* pointing at a blank char (space/tab), variable: - get variable */
	if (isblank(*line)) while (isblank(*line)) line++;


	/* assign variable */
	if (*line >= 'a' && *line <= 'z') {
		charvar = *line; line++;
	} else {
		printf("Error - bad variable name [%c] in LET\n",*line);
		return -1;
	}




	/* ****** Character Variable ****** */

	if (*line == '$') {			/* variable is a string */
		line++;					// skip $
		if (isblank(*line)) while (isblank(*line)) line++;
		if (*line != '=') {
			printf("Error - missing '=' in LET statement\n");
			return -1;
		}
		line++;			// skip =
		if (isblank(*line)) while (isblank(*line)) line++;
		
		/* assume everything else is an expression */
		/* if a comma is inside () then ignore it. Else it's a seperator */
		memset(tmpstr,0,sizeof(tmpstr)); n=0; int pc=0;	// parens count
		while (1) {
			if (*line == '\0' || *line == '\n') break;
			if (*line == ',' && pc==0) break;			// comma outside ()
			if (*line == '(') pc++;
			if (*line == ')') pc--;
			tmpstr[n] = *line;
			line++; n++;
		}
		if (pc > 0) {
			printf("Error - mismatched parenthesis in LET\n");
			return -1;
		}
		strcpy(CharVars[charvar-'a'],evalstring(tmpstr));
		if (error == -1) {
			printf("Error - bad string expression in LET statement\n");
			return -1;
		}
		// *line points to EOL or ','
		if (*line == '\0' || *line == '\n') return 0;
		if (*line == ',') {
			line++;
			goto letloop;
		}

		printf("Error - unknown character [%c] in LET statement\n",*line);
		return -1;
	}





	
	
	
	/* ***** Numeric Variable ****** */

	subscript = 0;				/* default subsript for array - see if different */
	if (*line == '(') {			/* variable is a numeric array */
		line++;		// skip (
		memset(tmpstr,0,sizeof(tmpstr));
		n=0;
		while (*line != ')') {
			tmpstr[n] = *line;
			line++; n++;
		}
		line++;			// skip )
		res = eval(tmpstr);
		if (error == -1) {
			printf("Error - subscript evaluation failed in LET statement\n");
			return -1;
		}
		subscript = (int)res;
		/* bounds testing */
		if (subscript > NumSize[charvar-'a'] || subscript < 0) {
			printf("Error - bad subscript [%d] in LET statement\n",subscript);
			return -1;
		}
	}

	/* if space, skip */
	if (isblank(*line)) while (isblank(*line)) line++;

	/* check for '=' */
	if (*line != '=') {
		printf("let: *expr [%c]\n",*line); 
		printf("Error - missing '=' in LET statement\n");
		return -1;
	}
	line++;		// skip =
	
	/* if space, skip */
	if (isblank(*line)) while (isblank(*line)) line++;

	/* get the expression, assign to the variable */
	memset(tmpstr,0,sizeof(tmpstr));
	n=0;
	while (1) {
		if (isblank(*line)) {
			line++;
			continue;
		}
		if (*line == ',') break;
		if (*line == '\0' || *line == '\n') break;
		tmpstr[n] = *line;
		line++; n++;
	}
	/* tmpstr holds expression */
	res = eval(tmpstr);
	if (error == -1) {
		printf("Error - bad expression [%s] in LET statement\n",tmpstr);
		return -1;
	}
	NumVar[charvar-'a'][subscript] = res;

	/* test for EOF */
	if (*line == '\0' || *line == '\n') return 0;

	/* test for ',' - more assignments on the line */
	if (*line == ',') {
		line++;
		goto letloop;
	}

	printf("Error - unknown character [%c] in LET statement\n",*line);
	return -1;
}



	
