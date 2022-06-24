/* insert.c
 *
 * part of dbasic
 *
 * (C) k theis <theis.kurt@gmail.com> 2022
 *
 * edit routines for interactive dbasic
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dbasic.h"


/* scan thru buffer, return the highest line number used */
int gethighestlinenumber() {
	extern char *buffer;
	extern int pos;
	int n=0, tmppos=0, linenumber = 0;
	char linenum[LNSIZE] = {'\0'};
	char line[LINESIZE] = {'\0'}; memset(line,0,LINESIZE);
	// read a line, get line number
ghln:
	n = 0; 
	while(1) {
		if (buffer[tmppos+n]=='\n' || buffer[tmppos+n]=='\0') break;
		line[n] = buffer[tmppos+n];
		n++;
	}
	sscanf(line,"%s ",linenum);
	linenumber = atoi(linenum);
	tmppos += n+1;		// skip final \n
	if (tmppos < pos) goto ghln;
	return linenumber;
}


/* return the starting address of the line 
 * where the linenumber is just above the given
 * linenumber
 */ 
int getaddressbeforelinenumber(int lnr) {
	extern char *buffer;
	extern int pos;

	int n=0, tmppos=0, linenumber=0;
	char linenum[LNSIZE];
	char line[LINESIZE]; memset(line,0,LINESIZE);
	// read a line, get line number and address. 
	// As soon as the line number is greater than 
	// the given line number, return the address;
gln:
	n = 0;
	while (1) {
		if (buffer[tmppos+n]=='\n' || buffer[tmppos+n]=='\0') break;
		line[n] = buffer[tmppos+n];
		n++;
	}
	sscanf(line,"%s ",linenum);
	linenumber = atoi(linenum);
	if (linenumber > lnr) return tmppos;
	tmppos += n+1;
	if (tmppos < pos) goto gln;
	return -1;
}



/* insert or delete a line */
int insert(char line[LINESIZE]) {
	
	/*
	 * If a line with a linenumber only is given, delete the line from the buffer
	 * If a line if given with a linenumber greater than the last line, append
	 * that line to the buffer.
	 * If a line with a linenumber somewhere in the middle of the buffer is given,
	 * insert that line into the buffer.
	 *
	 */


	extern int getstartaddress(int);
	extern int pos;
	extern char *buffer;
	char ch;
	int i=0, n=0, QFLAG=0;
	int linestart=0, lineend=0, linesize=0;

	if (strlen(line) > (BUFSIZE-pos)) {
		printf("Error - no space left\n");
		return -1;
	}

	// convert all except between "" to lower case
	QFLAG=0; n=0;
	while (1) {
		ch = line[n];
		if (ch == '\n' || ch == '\0') break;
		if (ch == '"')
			QFLAG = abs(QFLAG-1);
		if (!QFLAG)
			line[n] = tolower(ch);
		n++;
		continue;
	}

	// test if line has a line number
	char linenum[LNSIZE], keyword[LINESIZE];
	sscanf(line,"%s %s",linenum, keyword);
	if (atoi(linenum) < 1) {
		printf("Error - missing line number\nReady.\n");
		return -1;
	}

	// test if line number is a match
	int startaddress = getstartaddress(atoi(linenum));

	// line number not in buffer- see where to place it
	if (startaddress == -1) {	
		
		// first test for an empty line (not allowed)
		n=-1;
		while (1) {
			n++;
			if (isdigit(line[n])) continue;
			if (line[n] == '\0' || line[n] == '\n') return 0;
			if (isblank(line[n])) continue;
			break;
		}

		// test for end-of-buffer insert
		if (gethighestlinenumber() < atoi(linenum)) {
			for (i=0; i<strlen(line); i++) {
				buffer[pos+i] = line[i];
			}
			pos += i;	// and increment the end of the buffer
			return 0;
		}
		
		// insert somewhere in the buffer
		startaddress = getaddressbeforelinenumber(atoi(linenum));
		
		// insert the line at this address
		linesize = strlen(line);
        n = 0;
        while (1) {     // shift everything up by line length
            buffer[pos+linesize-n] = buffer[pos-n];
            n++;
            if (pos-n == startaddress-1) break;
        }
        pos += linesize;        // update end of buffer
        
		// insert new line at startaddress
        for (n=0; n<linesize; n++)
            buffer[startaddress+n] = line[n];

		return 0;
	}

		
	linestart = startaddress;
	// get lineend address
	for (i=startaddress; i<startaddress+LINESIZE; i++) {
		if (buffer[i] == '\n') break;
	}
	i++;		// skip \n
	lineend = i;
	
	// get size of existing line
	linesize = i-linestart+1;

	// shift down everything, deleting old line
	for (n=0; n<pos; n++)
		buffer[linestart+n] = buffer[lineend+n];
	pos = pos-linesize+1;
	if (strlen(line) - strlen(linenum) == 1) 
		return 0;			// user hit line# and return, delete line and done

	// insert changed line
	if (strlen(line) - strlen(linenum) > 1) {
		linesize = strlen(line);
		n = 0;
		while (1) {		// shift everything up by line length
			buffer[pos+linesize-n] = buffer[pos-n];
			n++;
			if (pos-n == linestart-1) break;
		}
		pos += linesize;		// update end of buffer
		
		// insert new line at startaddress
		for (n=0; n<linesize; n++)
			buffer[startaddress+n] = line[n];
	}
	
	return 0;
}
	
