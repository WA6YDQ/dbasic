/* goto.c
 *
 * part of dbasic
 *
 * (C) k theis <theis.kurt@gmail.com> 2022
 *
 * GOTO and ON GOTO, GOSUB and ON GOSUB routines
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dbasic.h"

/* GOTO */
int run_goto(char *line) {		// get line number, return it
	char ln[LNSIZE], word[6], linenum[LNSIZE];
	float eval(char *);

	sscanf(line,"%s %s %s",ln,word,linenum);

	/* return value of expression after 'goto' as the line number */
	return ((int)(eval(linenum)));
}

/* ON n GOTO x y z */
int run_ongoto(char *line) {	// return line number based on variable

	/* tokenize all chars on the line, verify 
	 * on, goto are correct, evaluate the variable,
	 * determine the line numbers and return the
	 * correct one or 0 if no match.
	 */

	//char *opers; 
	char *tmp;
	char temp[30][LNSIZE];			// max 30 line numbers
	int n=0, res=0;
	float eval(char *);
	
	/* tokenize the line */
	tmp = strtok(line," ");
	while ((tmp = strtok(NULL," "))!=NULL) {		// break on NULL
		if (tmp != NULL) strcpy(temp[n],tmp);
		n++;
		if (n > 30) {
			printf("Error - max of 30 addresses allowed\n");
			return -1;
		}
	}
	// element 0 is 'on'
	
	// element 1 is the variable
	res = (int)(eval(temp[1])); 
	if (res > n) return 0;		// only 30 line numbers allowed

	// element2 is 'goto'
	if (!(strcmp(temp[2],"goto")==0 || strcmp(temp[2],"gosub")==0)) {
		printf("Error - expected GOTO/GOSUB in statement\n");
		return -1;
	}
	// elements 3...(end) are line numbers
	return atoi(temp[res+2]);
	// if return value < 1 or > highest number, return 0
	
}
