/* goto line number */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int run_goto(char *line) {		// get line number, return it
	char ln[6], word[6], linenum[40];
	int eval(char *);

	sscanf(line,"%s %s %s",ln,word,linenum);

	/* return value of expression after 'goto' as the line number */
	return (eval(linenum));
}

