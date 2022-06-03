/* basic.c - Kurt's version of Dartmuth BASIC

 Statements in this version of BASIC:
 REM, PRINT, LET, GOTO, GOSUB, RETURN, END, 
 IF/THEN, FOR/NEXT/STEP, INPUT, READ, DATA,
 RESTORE, ON/GOTO

 Integer numeric variables are a...z
 String variables are a$...z$

 All keywords, variables are in lower case.
 Case is preserved when between double quotes ""

 TODO: 
 string functions: LEFT$(), MID$(), RIGHT$()
 trig functions: SIN(), COS(), TAN(), ATN()
 math functions: INT(), RND(), FN(), SGN()
 misc functions: TAB()
 Floating Point math, arrays, DIM()

 (C) 2022 Kurt Theis <theis.kurt@gmail.com>
 This is licensed using the MIT license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFSIZE 65536
#define LINESIZE 80

/* globals */
char *buffer;			// memory buffer holding the basic program
int NumericVars[26];	// numeric integer variables a-z
char CharVars[26][LINESIZE];	// a$ thru z$, max 80 chars long each
int pos = 0;			// current end position of the buffer
int currentlinenumber = 0;	// line number of the current line being tested
int lineptr = 0;		// current buffer position during run loop
int returnlinenumber = 0;	// position in buffer when RETURN command issued
int index_start = 0;	// FOR/NEXT start index
int index_end = 0;		// FOR/NEXT end index
int index_step = 0;		// FOR/NEXT step index
char index_var;			// FOR/NEXT count variable
int forlinenumber = 0;	// FOR/NEXT line number to return to
int *DataStorage;		// DATA statement storage, will assign later
int DataStorageMax=0;	// max number of data elements
int DataPosition=0;		// position of read data objects


/* extern functions */
int parse(char *);
int run_let(char *);
int run_print(char *);
int run_input(char *);
int run_goto(char *);
int run_gosub(char *);
int eval(char *);
int getstartaddress(int);
int run_ifthen(char *);
int run_for(char *);
int run_next(char *);
int scanData(char *,int);
int run_read(char *);
int run_ongoto(char *);


/***** RUN_CLEAR ******/
/* set all vars to 0 */
int run_clear(void) {
	for (int n=0; n<26; n++) 
		NumericVars[n] = 0;
	// set index vars to 0
	index_start = 0;
	index_end = 0;
	index_step = 0;
	index_var = ' ';
	forlinenumber = 0;
	// set char vars to NULL
	for (int n=0; n<=26; n++) memset(CharVars[n],0,LINESIZE);
	return 0;
}

/******** GETSTARTADDRESS ************/
int getstartaddress(int linenum) {
	
/*
	Search for the supplied line number
	in the buffer. If found, return the 
	starting address of the line. If not
	found, or if an error, return -1.

*/
	char testline[LINESIZE];
	char numstr[6] = {};
	int n=0, addr=0; 
gsrloop:
	n=0;
	memset(testline,0,LINESIZE);
	while (1) {
		testline[n] = buffer[addr+n];
		if (testline[n] == '\n') break;
		n++;
		if (n>LINESIZE) return -1;		// should never happen
	}
	testline[n] = '\0';
	sscanf(testline,"%s",numstr);
	if (atoi(numstr)==linenum) return addr;		// match
	addr += (n+1);								// skip \n
	if (addr >= pos-1) return -1;				// no match
	goto gsrloop;
}



/******************** PARSE *************************/
/* read a line, parse it and execute basic commands */
/* normal commands return a 0                       */
/* end returns -999                                 */
/* goto/gosubs return a line number                 */
/* (0 and -1 are bad line numbers)                  */
/****************************************************/

int parse(char *line) {
	int res = 0;
	char linenum[6],word[40];
	sscanf(line,"%s %s",linenum,word);	// get line number and 1st command
	//printf("[%s]\n",line);
	
	if (strcmp(word,"rem")==0) {		// ignore remarks
		return 0;
	}

	if (strcmp(word,"data")==0) {		// ignore data statements (handled elsewhere)
		return 0;
	}

	if (strcmp(word,"read")==0) {		// data.c
		res = run_read(line);
		return res;
	}

	if (strcmp(word,"restore")==0) {	// reset data pointer to 0
		DataPosition = 0;
		return 0;
	}

	if (strcmp(word,"let")==0) {		// let.c
		res = run_let(line);
		return res;
	}
	
	if (strcmp(word,"print")==0) {		// print.c
		res = run_print(line);
		if (res != 0) {
			printf("line %s\n",linenum);
			return -1;
		}
		return 0;
	}
	if (strcmp(word,"input")==0) {		// input.c
		run_input(line);
		return 0;
	}
	if (strcmp(word,"end")==0) {		// end the program
		return -999;
	}
	if (strcmp(word,"clr")==0) {		// clear numeric variables
		run_clear();
		return 0;
	}
	if (strcmp(word,"goto")==0) {		// goto.c
		return run_goto(line);			// return the line number
	}
	if (strcmp(word,"on")==0) {			// goto.c
		return run_ongoto(line);
	}
	if (strcmp(word,"gosub")==0) {		// gosub.c
		/* save the line number following this line */
		int i=lineptr, x=0;				// skip current line
		while (buffer[i++] != '\n');
		char tmpline[LINESIZE]={};		// next line
		while (1) {
			tmpline[x] = buffer[i+x];	// fill temp line
			if (tmpline[x]=='\n') break;
			x++;
		}
		char linen[6];
		sscanf(tmpline,"%s",linen);		// get line number of next line
		returnlinenumber = atoi(linen);
		return run_gosub(line);			// return the line number
	}
	if (strcmp(word,"return")==0) {		// return from subroutine
		//printf("return: retline is %d\n",returnlinenumber);
		return returnlinenumber;
	}
	if (strcmp(word,"if")==0) {			// IF/THEN: ifthen.c
		return run_ifthen(line);
	}
	if (strcmp(word,"for")==0) {		// for/next  fornext.c
		// get/save the line # of the next line
		int i=lineptr, x=0;
		while (buffer[i++] != '\n');
		char tmpline[LINESIZE]={};
		while (1) {
			tmpline[x] = buffer[i+x];
			if (tmpline[x]=='\n') break;
			x++;
		}
		//printf("for: next line [%s]\n",tmpline);
		char linen[6];
		sscanf(tmpline,"%s",linen);
		forlinenumber = atoi(linen);
		//printf("for: forlinenumber = %d\n",forlinenumber);
		return run_for(line);
	}
	if (strcmp(word,"next")==0) {		// for/next  fornext.c
		/* increment index_var by index_step. if index_var > index_end skip jump */
		if (index_var == ' ') {
			printf("Error - NEXT without FOR in line %d\n",currentlinenumber);
			exit(0);
		}
		NumericVars[index_var - 'a'] += index_step;
		if (index_step > 0) { 
			if (NumericVars[index_var - 'a'] > index_end) 
				return 0;
		} else {
			if (NumericVars[index_var - 'a'] < index_end) 
					return 0;
		}
		return forlinenumber;
		
	}

	// unknown command
	printf("SYNTAX ERROR: [%s]\n",line);
	return 0;
}


/********* MAIN **********/
int main(int argc, char **argv) {

	int n=0;				// local var
	int linecount=0;		// number of program lines
	char line[LINESIZE];
	char filename[LINESIZE]={};

	/* load a file into memory */
	if (argc == 1) {		// no args given
		printf("Filename? ");
		fgets(filename,LINESIZE,stdin);
		if (filename[0]=='\r' || filename[0]=='\n') {	// exit program
			free(buffer);
			return 1;
		}

	} else {
		strcpy(filename,argv[1]);
	}

	// filename given
	for (n=0; n<strlen(filename); n++) 
		if (filename[n]=='\n' || filename[n]=='\r') 
			filename[n]='\0';
	FILE *fd = fopen(filename,"r");
	if (fd == NULL) {
		printf("Unable to open %s\n",filename);
		free(buffer);
		return 1;
	}

	/* set up memory for the file */
	buffer = malloc(BUFSIZE*sizeof(char));
    if (buffer == NULL) {
        printf("memory error\n");
        return 1;
    }
	memset(buffer,0,BUFSIZE);

	// load a file to memory
	pos = 0;
	while (1) {
		memset(line,0,LINESIZE);
		fgets(line,LINESIZE,fd);
		if (feof(fd)) break;	// end of file
		linecount++;
		for (n=0; n<strlen(line); n++) buffer[pos+n] = line[n];
		pos += n;
		// bounds checking
		if (pos > (BUFSIZE-80)) {
			printf("Error - memory full, file load incomplete.\n");
			printf("Stopped at line %d\n",linecount-1);
			break;
		}
	}
	// done reading file
	fclose(fd);
	
	printf("File %s loaded. %d bytes free\n",filename,BUFSIZE-pos);
	printf("%d lines read\n",--linecount);

	/**************************************************/
	/******** START RUNNING THE BASIC PROGRAM *********/
	/**************************************************/

	/* 
	 read each line in the buffer, parse 
	 the statements, loop until END statement 
	 is reached or an error occurs.
	 
	*/
	
	int res = 0;		// result, function return value
	char ln[6];			// line number string placeholder
	lineptr = 0;		// initialize position in buffer
	run_clear();		// set numeric vars to 0 at start
						// clear string vars, for/next vars
	/* look for data statements, preload memory */
	res = scanData(buffer,pos);
	if (res == -1) {	// memory error - fatal
		free(buffer);
		exit(1);
	}
	DataPosition = 0;	// start at beginning of data list


parseLoop:
	memset(line,0,LINESIZE);
	n=0;
	res = 0;
	while (1) {
		/* extract a single line from the buffer */
		line[n] = buffer[lineptr+n];
		if (line[n]=='\n') break;
		n++;
		continue;
	}
	line[n]='\0';		// since memset sets all chars to 0, this just removes the \n
	
	/* extract the current line number (used for error messages) */
	sscanf(line,"%s",ln);
	currentlinenumber = atoi(ln);
	if (currentlinenumber <= 0) {	// error
		printf("Error - bad line number %s\n",ln);
		exit(0);
	}
	
	/* parse the BASIC statements, execute them */
	res = parse(line);
	
	/* test return code */
	if (res == -999) {				// END Statement
		printf("END statement in line %d\n",currentlinenumber);
		free(buffer);
		free(DataStorage);
		exit (0);
	}
	if (res == -1) {				// parse returned an error
		printf("Error in line %d\n",currentlinenumber);
		free(buffer);
		free(DataStorage);
		exit(0);
	}
	if (res > 0) {					// goto/gosub statement returns new line number
		/* find the matching line number, set lineptr to start of line */
		lineptr = getstartaddress(res);
		if (lineptr == -1) {			// bad line number
			printf("Line %d - goto/gosub: bad line number %d\n",currentlinenumber,res);
			free(buffer);
			free(DataStorage);
			exit(0);
		}
		goto parseLoop;				// continue at new address
	}

	// get next line
	lineptr += (n+1);
	if (lineptr >= pos-1) {		// end of file reached
		printf("End of file reached\n");
		free(buffer);
		free(DataStorage);
		exit(0);
	}
	goto parseLoop;				// continue with basic program

	
	/* all below should never be reached */
	printf("Unknown error in line %d.\n",currentlinenumber);

	/* done */
	free(buffer);
	free(DataStorage);
	return 0;
}


