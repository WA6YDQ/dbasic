/* ifthen.c - if (expression) =, !=, <, > (expression) then [line number] */
/* test 2 values - if they match the text chars, return a line number
 * else return 0 (failed test)
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dbasic.h"


int run_ifthen(char *line) {

	float eval(char *);
	float lvalue=0, rvalue=0, linenum=0;
	int n=0;
	char expr[LINESIZE] = {};					// expression for eval()
	
	//printf("line is [%s]\n",line);
	
	while (isdigit(*line)) line++;		// skip line number
	if (isblank(*line)) while (isblank(*line)) line++;		// and spaces
	while (isalpha(*line)) line++;		// and LET statement
	while (!isalpha(*line)) line++;		// skip spaces until 1st variable

	/* get 1st value for tests */
	memset(expr,0,sizeof(expr)); n = 0;
	while (1) {
		if (isblank(*line)) break;
		expr[n] = *line;
		n++; line++;
		if (n >= LINESIZE) {					// expression too long
			printf("Error - expression too long\n");
			return -1;
		}
	}
	lvalue = eval(expr);
	//printf("ifthen: lvalue=%d\n",lvalue);

	/* get test variable */
	int testchar = 0;
	// skip any spaces
	if (isblank(*line)) while (isblank(*line)) line++;
	if (*line == '<' && *(line+1)==' ') {
		testchar = 1;	// less than
		line += 1;
	}
	else if (*line == '<' && *(line+1)=='=') {
		testchar = 2;	// less than or equal
		line += 2;
	}
	else if (*line == '>' && *(line+1)==' ') {
		testchar = 3;	// greater than
		line += 1;
	}
	else if (*line == '>' && *(line+1)=='=') {
		testchar = 4;	// greater than or equal
		line += 2;
	}
	else if (*line == '=' && *(line+1)==' ') {
		testchar = 5;	// equal
		line += 1;
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
	//printf("ifthen: test variable = %d\n",testchar);


	/* get 2nd value for tests */
	if (isblank(*line)) while (isblank(*line)) line++;	// skip spaces
	memset(expr,0,sizeof(expr)); n=0;
	while (1) {
		if (isblank(*line)) break;
		expr[n] = *line;
		n++; line++;
		if (n >= LINESIZE) {					// expression too long
			printf("Error - expression too long\n");
			return -1;
		}
	}
	rvalue = eval(expr);
	//printf("ifthen: rvalue = %d\n",rvalue);

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
	//printf("ifthen: line number = %d\n",linenum);

	/* run test betweek values */
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

