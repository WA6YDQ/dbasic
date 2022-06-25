/* file.c - file operations for dbasic
 *
 * part of dbasic
 *
 * (C) 2022 K Theis <theis.kurt@gmail.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "dbasic.h"


/* open a file */
int fileopen(char *line) {
	extern FILE *fd[8];
	extern int fdnumber;
	
	/* format: linenum OPEN [filename] FOR mode AS #[n] */
	char ln[LNSIZE]={'\0'}, cmd[LINESIZE]={'\0'}, \
		o_filename[LINESIZE]={'\0'}, cmd2[LINESIZE]={'\0'}, \
		o_mode[LINESIZE]={'\0'},  cmd3[LINESIZE]={'\0'}, o_filenum[LNSIZE]={'\0'};

	sscanf(line,"%s %s %s %s %s %s %s",ln,cmd,o_filename,cmd2,o_mode,cmd3,o_filenum);
	/* ln and cmd pass or we wouldn't be here */
	if (strcmp(cmd,"for")==0) {
		printf("Error - expected 'for' keyword, got %s\n",cmd2);
		return -1;
	}
	
	if (!(strcmp(o_mode,"input")==0 || strcmp(o_mode,"output")==0 || strcmp(o_mode,"append")==0)) {
		printf("Error - bad mode: expected 'input', 'output' or 'append', got %s\n",o_mode);
		return -1;
	}

	if (strcmp(cmd3,"as")!=0) {
		printf("Error - expected 'as' keyword, got %s\n",cmd3);
		return -1;
	}

	/* extract filenumber */
	if (o_filenum[0] != '#') {
		printf("Error - badly formatted file number\n");
		return -1;
	}

	int n=0;
	while (o_filenum[n] != '\0') {
		o_filenum[n] = o_filenum[n+1];
		n+=1;
		if (n > LNSIZE) {
			printf("Error - runaway process: fileopen()\n");
			return -1;
		}
	}
	fdnumber = atoi(o_filenum);
	if (fdnumber < 1 || fdnumber > 7) {
		printf("Error - bad file number [%s]\n",o_filenum);
		return -1;
	}
	
	/* test if the file is already open */
	if (fd[fdnumber] != NULL) {
		printf("Error - file number %d is already in use.\n",fdnumber);
		return -1;
	}

	/* strip off any double quotes that may be on the filename */
	char filename[LINESIZE] = {'\0'}; n=0; int x=0;
	while (1) {
		if (o_filename[x] == '\0') break;
		if (o_filename[x] == '"') {
			x++;
			continue;
		}
		filename[n] = o_filename[x];
		x++; n++;
	}

	/* open/create file based on it's mode */
	if (strcmp(o_mode,"input")==0) {		// open for read
		fd[fdnumber] = fopen(filename,"r");
	}

	if (strcmp(o_mode,"output")==0) {		// open for write
		fd[fdnumber] = fopen(filename,"w");
	}

	if (strcmp(o_mode,"append")==0) {		// open for append
		fd[fdnumber] = fopen(filename,"a+");
	}

	if (fd[fdnumber] == NULL) {
            printf("Error - could not open file %s for %s\n",filename,o_mode);
            return -1;
    }

	return 0;
}


/* close an open file */
int fileclose(char *line) {

	extern FILE *fd[8];
	extern int fdnumber;

	/* format: linenum close #[n] */
	char ln[LNSIZE]={'\0'}, cmd[LINESIZE]={'\0'}, o_filenum[LNSIZE]={'\0'};

	sscanf(line,"%s %s %s",ln,cmd,o_filenum);

	/* extract filenumber */
    if (o_filenum[0] != '#') {
        printf("Error - badly formatted file number\n");
        return -1;
    }

    int n=0;
    while (o_filenum[n] != '\0') {
        o_filenum[n] = o_filenum[n+1];
        n+=1;
		if (n > LNSIZE) {
			printf("Error - runaway process: fileclose()\n");
			return -1;
		}
    }
    fdnumber = atoi(o_filenum);
    if (fdnumber < 1 || fdnumber > 7) {
        printf("Error - bad file number [%s]\n",o_filenum);
        return -1;
    }

    /* test if the file is already closed */
    if (fd[fdnumber] == NULL) {
		printf("Error - file #%d is already closed\n",fdnumber);
		return -1;
	}

	/* close the file */
	fflush(fd[fdnumber]);
	fclose(fd[fdnumber]);
	fd[fdnumber] = NULL;


	return 0;
}

/* rewind an open file */
int filerewind(char *line) {

	extern FILE *fd[8];
	extern int fdnumber;

	/* format linenum rewind #[n] */
	char ln[LNSIZE]={'\0'}, cmd[LINESIZE]={'\0'}, o_filenum[LNSIZE]={'\0'};

    sscanf(line,"%s %s %s",ln,cmd,o_filenum);

	/* extract filenumber */
    if (o_filenum[0] != '#') {
        printf("Error - badly formatted file number\n");
        return -1;
    }

    int n=0;
    while (o_filenum[n] != '\0') {
        o_filenum[n] = o_filenum[n+1];
        n+=1;
        if (n > LNSIZE) {
            printf("Error - runaway process: rewind()\n");
            return -1;
        }
    }
    fdnumber = atoi(o_filenum);
    if (fdnumber < 1 || fdnumber > 7) {
        printf("Error - bad file number [%s]\n",o_filenum);
        return -1;
    }

    /* test if the file is already closed */
    if (fd[fdnumber] == NULL) {
        printf("Error - file #%d is closed\n",fdnumber);
        return -1;
    }

	/* rewind the file */
	rewind(fd[fdnumber]);

	return 0;
}
