/* ifthen.c - if (expression) =, !=, <, > (expression) then [line number] */
/* test 2 values - if they match the text chars, return a line number
 * else return 0 (failed test)
 * 
 * This tests both numeric and string expressions
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

/* return 1 on valid operand */
int isoperand(char ch) {
	if (ch == '<' || ch == '>' || ch == '!' || ch == '=') return 1;
	return 0;
}

int run_ifthen(char *line) {

	float eval(char *);
	char *evalstring(char *);
	float lvalue=0, rvalue=0, linenum=0;
	int n=0;
	char expr[LINESIZE] = {'\0'};					// expression for eval()
	int isoperand(char);
	extern int error;
	int STRFLAG1=0, STRFLAG2=0;						// set if $ found in expression
	char leftchar[LINESIZE] = {'\0'};
	char rightchar[LINESIZE] = {'\0'};

	// debug
	// printf("line is [%s]\n",line);
	
	while (isdigit(*line)) line++;		// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;		// and spaces
	while (isalpha(*line)) line++;		// and LET statement
	while (!isalpha(*line)) line++;		// skip spaces until 1st variable

	/* get 1st value for tests */
	memset(expr,0,sizeof(expr)); n = 0; int pc=0;
	while (1) {
		if (isblank(*line) && pc==0) break;
		if (isoperand(*line) && pc==0) break;
		if (*line == '\0' || *line == '\n') {
			printf("Error - missing expression in IF statement\n");
			return -1;
		}
		if (*line == '(') pc++;
		if (*line == ')') pc--;
		if (*line == '$' || *line == '"') STRFLAG1=1;
		expr[n] = *line;		// ignore "" for chars
		n++; line++;
		if (n >= LINESIZE) {					// expression too long
			printf("Error - expression too long in IF\n");
			return -1;
		}
	}

	if (pc != 0) {
		printf("Error - mismatched () in IF/THEN\n");
		return -1;
	}

	if (!STRFLAG1) lvalue = eval(expr);		// numeric expression
	if (STRFLAG1) strcpy(leftchar,evalstring(expr));			// string expression
	if (error == -1) return error;				// test eval()

	/* get test variable */
	int testchar = 0;
	// skip any spaces
	if (isblank(*line)) while (isblank(*line)) line++;
	
	if (isoperand(*line) && !isoperand(*(line+1))) {
		if (*line == '<') 
			testchar = 1;
		else if (*line == '>')
			testchar = 3;
		else if (*line == '=')
			testchar = 5;
		line++;
	} else if (*line == '<' && *(line+1)=='=') {
		testchar = 2;	// less than or equal
		line += 2;
	}
	else if (*line == '>' && *(line+1)=='=') {
		testchar = 4;	// greater than or equal
		line += 2;
	}
	else if (*line == '!' && *(line+1)=='=') {
		testchar = 6;	// not equal
		line += 2;
	}
	else if (*line == '<' && *(line+1)=='>') {
		testchar = 7;	// not equal
		line += 2;
	}
	else {
		printf("Unknown test character %c\n",*line);
		return -1;
	}


	/* get 2nd value for tests */
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces
	memset(expr,0,sizeof(expr)); n=0; pc=0;
	while (1) {
		if (isblank(*line) && pc==0) break;
		if (*line == '$' || *line == '"') STRFLAG2=1;			// is a string 
		if (*line == '(') pc++;
		if (*line == ')') pc--;
		if (*line == '\0' || *line == '\n') {
			printf("Error - bad value in IF/THEN\n");
			return -1;
		}
		expr[n] = *line;		// ignore "" for strings
		n++; line++;
		if (n >= LINESIZE) {					// expression too long
			printf("Error - expression too long\n");
			return -1;
		}
	}

	if (!STRFLAG2) rvalue = eval(expr);			// numeric expression
	if (STRFLAG2) strcpy(rightchar,evalstring(expr));			// string expression
	if (error == -1) return error;

	/* get THEN statement */
	if (isblank(*line)) while (isblank(*line)) line++;
	if (*line == 't' && *(line+1) == 'h' && *(line+2) == 'e' && *(line+3) == 'n') {
		line += 4;		// skip "THEN"
	} else {
		printf("Error - missing THEN in IF/THEN\n");
		return -1;
	}

	/* get return line number */
	if (isblank(*line)) while (isblank(*line)) line++;
	memset(expr,0,sizeof(expr)); n = 0;
	while (1) {
		expr[n] = *line;
		if (isblank(*line)) break;
		if (*line == '\n') break;
		if (*line == '\0') break;
		n++; line++;
		if (n > LINESIZE) {					// expression too long
			printf("Error - expression too long\n");
			return -1;
		}
	}
	linenum = eval(expr);


	/* test if both expressions are strings */
	if ((STRFLAG1 == STRFLAG2) && STRFLAG1) {		// this is a string test
		int strres = strcmp(leftchar,rightchar);
		if (testchar == 5 && strres == 0) return linenum;	// 1st = 2nd
		if (testchar == 1 && strres == -1) return linenum;	// 1st < 2nd
		if (testchar == 2 && (strres==-1 || strres==1)) return linenum;	// 1st <= 2nd
		if (testchar == 3 && (strres == 1)) return linenum;		// 1st < 2nd
		if (testchar == 4 && (strres==0 || strres==1)) return linenum;	// 1st <= 2nd
		if (testchar >= 6 && (strres != 0)) return linenum;		// 1st != 2nd
		return 0;
	}

	/* test unequal expressions (string/numeric) */
	if (STRFLAG1 != STRFLAG2) {
		printf("Error - string to numeric expression test in IF/THEN\n");
		return -1;
	}


	/* test numeric expressions */
	if (testchar == 1) 			// less than
		if (lvalue < rvalue) return linenum;
	if (testchar == 2) 			// less than or equal
		if (lvalue <= rvalue) return linenum;
	if (testchar == 3) 			// greater than
		if (lvalue > rvalue) return linenum;
	if (testchar == 4)			// greater than or equal
		if (lvalue >= rvalue) return linenum;
	if (testchar == 5)			// equal
		if (lvalue == rvalue) return linenum;
	if (testchar == 6 || testchar == 7)	// not equal
		if (lvalue != rvalue) return linenum;
	// none of the above
	return 0;
}

