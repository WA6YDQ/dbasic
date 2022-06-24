/* loadsave.c - load a file to buffer, save the buffer to file 
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


void save(char *filename) {
	extern char *buffer;
	extern int pos;

	FILE *fd = NULL;
	fd = fopen(filename,"w");
	if (fd == NULL) {
		printf("Error - unable to create file %s\n",filename);
		return;
	}
	for (int n=0; n<pos; n++)
		fprintf(fd,"%c",buffer[n]);
	fflush(fd);
	fclose(fd);
	printf("File %s Saved\n",filename);
	return;
}

void load(char *filename) {
	extern char *buffer;
	extern int pos;
	int linecount = 0;
	int n=0;
	char ch;
	int QFLAG=0;
	FILE *fd = NULL;
	
	fd = fopen(filename,"r");
	if (fd == NULL) {
		printf("Error - unable to read file %s\n",filename);
		return;
	}
	pos = 0; memset(buffer,0,BUFSIZE);
	while (1) {
		ch = fgetc(fd);
		if (feof(fd)) break;
		buffer[pos++] = ch;
		if (ch == '\n') linecount++;
	}
	fclose(fd);

	/* The source can be in UPPER CASE or lower case. This
	 * part of the load operation converts all except between
	 * double quotes to lower case
	 */
	n = 0; QFLAG = 0;
	while (1) {
		ch = buffer[n];
		if (ch == '"') 
			QFLAG = abs(QFLAG-1);
		if (!QFLAG)
			buffer[n] = tolower(ch);
		if (++n < pos) continue;
		break;
	}

	printf("File %s loaded\n",filename);
	printf("Read in %d characters, %d lines\n",pos,linecount);
	return;
}
