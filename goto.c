/* goto line number */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* GOTO */
int run_goto(char *line) {		// get line number, return it
	char ln[6], word[6], linenum[40];
	int eval(char *);

	sscanf(line,"%s %s %s",ln,word,linenum);

	/* return value of expression after 'goto' as the line number */
	return (eval(linenum));
}

/* ON n GOTO x y z */
int run_ongoto(char *line) {	// return line number based on variable

	/* tokenize all chars on the line, verify 
	 * on, goto are correct, evaluate the variable,
	 * determine the line numbers and return the
	 * correct one or 0 if no match.
	 */

	char *opers; 
	char *tmp;
	char temp[30][20];			// max 30 line numbers
	int n=0, res=0, retln=0;
	int eval(char *);

	/* tokenize the line */
	opers = strtok(line," ");
	while (tmp = strtok(NULL," ")) {
		if (tmp != NULL) strcpy(temp[n],tmp);
		n++;
	}
	
	// element 0 is 'on'
	
	// element 1 is the variable
	res = eval(temp[1]); 
	if (res > n) return 0;		// only 30 line numbers allowed

	// element2 is 'goto'
	if (strcmp(temp[2],"goto")!=0) {
		printf("Error - expected GOTO in ON/GOTO\n");
		return -1;
	}
	
	// elements 3...(end) are line numbers
	return atoi(temp[res+2]);
	// if return value < 1 or > highest number, return 0
	
}
