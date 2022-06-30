/* basic.c - Kurt's version of Dartmuth BASIC
   see the file Dbasic.txt for usage instructions
   (C) 2022 K Theis <theis.kurt@gmail.com>

 This is free software under the MIT license.

 Statements in this version of BASIC:
 REM, PRINT, LET, GOTO, GOSUB, RETURN, END, 
 IF/THEN, FOR/NEXT/STEP, INPUT, READ, DATA,
 RESTORE, ON/GOTO, ON/GOSUB, CLEAR, STOP, DEF,
 OPEN, CLOSE, REWIND

 Formulas: a+b, a-b, a*b, a/b, a**b, -a
 
 Equality Tests: >, >=, <, <=, =, <>, !=
 
 Logical:  a&b, a|b, a^b (and/or/xor)
 
 Functions: SIN(), COS(), TAN(), EXP(), LN(),
 LOG(), SQR(), LEFT$(), RIGHT$(), MID$(), ASC(),
 CHR$(), FNx, INT(), ABS(), SGN(), ATN(), LEN(),
 RND(), RAD(), DEC(), SPC(), VAL(), EOF(), STR$(),
 TIME()

 Floating point numeric variables are a...z
 a(0)..a(10) ... z(0)..z(10) are pre-defined
 String variables are a$...z$

 All keywords, variables are in lower case.
 Case is preserved when between double quotes ""

 Please see the file Dbasic.txt in this repository
 for details on use, example programs, etc.


 TODO: 
 keywords:
 string functions: 
 math functions: matrix math,
 misc functions: NOT(), TAB(), AND(), OR()


	To build this:
	make
	To install:
	sudo make install
	binary 'dbasic' will be installed in /usr/local/bin

	To start basic, type 'dbasic'.
	Type 'exit' or 'quit' to exit basic.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include "dbasic.h"
#define _ISOC99_SOURCE
#include <math.h>
#include <time.h>

/* global variables */
char *buffer;					// memory buffer holding the basic program
char CharVars[26][LINESIZE];	// a$ thru z$, max 80 chars long each
int pos = 0;					// current end position of the buffer
int currentlinenumber = 0;		// line number of the current line being tested
int lineptr = 0;				// current buffer position during run loop
// position in buffer when RETURN command issued
int returnlinenumber[GOSUBSTACKSIZE];
int returnPos = 0;				
float index_start = 0;			// FOR/NEXT start index
float index_end = 0;			// FOR/NEXT end index
float index_step = 0;			// FOR/NEXT step index
char index_var;					// FOR/NEXT count variable
int forlinenumber = 0;			// FOR/NEXT line number to return to
float *DataStorage;				// DATA statement storage, will assign later
int DataStorageMax=0;			// max number of data elements
int DataPosition=0;				// position of read data objects
char fn[26][LINESIZE]={'\0'};				// function definitions
int error = 0;					// used by eval(), 0 is OK, -1 is expression error
char tempCharVar[MAXLINESIZE];		// used in evalstring()
int RUNSTOP=0;					// ctrl-C handler
long int BASICSTART = 0l;		// time (in seconds) since basic started

// these are file descripters
FILE *fd[8];
int fdnumber=0;

// these reference numeric variables
int GARBAGE = 0;				// numeric variable garbage collection
float *NumBase[26];				// the pointers that hold actual numeric variables a..z
float *NumVar[26];				// the pointer used in other functions to reference a..z
int NumSize[26];				// used in DIM(). Stops from re-diming downwards.

/* extern functions */
int parse(char *);
int run_print(char *);
int run_input(char *);
int run_goto(char *);
int run_gosub(char *);
float eval(char *);
int getstartaddress(int);
int run_ifthen(char *);
int run_for(char *);
int run_next(char *);
int scanData(char *,int);
int run_read(char *);
int run_ongoto(char *);
int def(char *);
int insert(char *);
void load(char *);
void save(char *);
int dim(char *);
int let(char *);
int fileopen(char *);
int fileclose(char *);
int filerewind(char *);
int version(void);

/**** VERSION() *****/
int version(void) {		// show version
	printf("\ndbasic version %s\n",VERSION);
	printf("(C) 2022 Kurt Theis <theis.kurt@gmail.com>\n");
	return 0;
}

/***** RUN_CLEAR ******/
/* set all vars to 0 when program starts */
int run_clear(void) {
	extern float *NumVar[26];
	extern char fn[26][LINESIZE];
	extern float index_start, index_end, index_step;
	extern char index_var;
	extern int forlinenumber;
	extern char CharVars[26][LINESIZE];
	extern int DataPosition;
	extern int DataStorageMax;
	extern char tempCharVar[MAXLINESIZE];

	// clear data statements
	DataPosition=0;
	DataStorageMax = 0;

	// clear numeric vars
	for (int row=0; row<26; row++) {
		for (int col=0; col<NumSize[row]; col++) {
			NumVar[row][col] = 0;
		}
	}
	
	// set index vars to 0
	index_start = 0;
	index_end = 0;
	index_step = 0;
	index_var = ' ';
	forlinenumber = 0;
	
	// set char vars to NULL
	for (int n=0; n<26; n++) memset(CharVars[n],0,LINESIZE);
	memset(tempCharVar,0,MAXLINESIZE);

	// set functions to null
	for (int n=0; n<26; n++) memset(fn[n],0,LINESIZE);
	
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
	char numstr[LNSIZE] = {'\0'};
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


/***** GETNEXTLINENUMBER *******/
int getNextLineNumber() {
	/* get & save the line number following the current line */
	/* This only works when called from the parse loop */
    int i=lineptr, x=0;             // skip current line
    while (buffer[i++] != '\n');
    char tmpline[LINESIZE]={'\0'};      // next line
    while (1) {
    	tmpline[x] = buffer[i+x];   // fill temp line
        if (tmpline[x]=='\n') break;
            x++;
    }
    char linen[LNSIZE];
    sscanf(tmpline,"%s",linen);     // get line number of next line
    return atoi(linen);
}


/******************** PARSE *************************/
/* read a line, parse it and execute basic commands */
/* normal commands return a 0                       */
/* end returns -999, stop returns -998              */
/* goto/gosubs return a line number                 */
/* -1 is an error, 0 is normal return               */
/****************************************************/

int parse(char *line) {
	int res = 0;
	char linenum[LNSIZE],word[LINESIZE], word2[LINESIZE],word3[LINESIZE];
	sscanf(line,"%s %s",linenum,word);	// get line number and 1st command

	if (strcmp(word,"ver")==0) {		// show version
		return version();
	}

	if (strcmp(word,"rem")==0) {		// ignore this line
		return 0;
	}

	if (strcmp(word,"dim")==0) {		// dim.c   re-dimension numeric variable
		res = dim(line);
		return res;
	}

	if (strcmp(word,"data")==0) {		// data.c   ignore data statements (handled elsewhere)
		return 0;
	}

	if (strcmp(word,"read")==0) {		// data.c
		res = run_read(line);
		return res;
	}

	if (strcmp(word,"restore")==0) {	// data.c    reset data pointer to 0
		DataPosition = 0;
		return 0;
	}

	if (strcmp(word,"def")==0) {		// def.c    def fnx
		res = def(line);
		return res;
	}
	
	if (strcmp(word,"open")==0) {		// file.c
		res = fileopen(line);
		return res;
	}

	if (strcmp(word,"close")==0) {		// file.c
		res = fileclose(line);
		return res;
	}

	if (strcmp(word,"rewind")==0) {		// file.c
		res = filerewind(line);
		return res;
	}

	if (strcmp(word,"let")==0) {		// let.c
		res = let(line);
		if (error == 0) return res;
		return -1;
	}
	
	if (strcmp(word,"print")==0) {		// print.c
		res = run_print(line);
		return res;
	}
	if (strcmp(word,"input")==0) {		// input.c
		res = run_input(line);
		return res;
	}
	if (strcmp(word,"stop")==0) {		// stop execution
		return -998;
	}
	if (strcmp(word,"end")==0) {		// end the program
		return -999;
	}
	if (strcmp(word,"clear")==0) {		// clear numeric variables
		run_clear();
		return 0;
	}
	if (strcmp(word,"goto")==0) {		// goto.c
		return run_goto(line);			// return the line number
	}
	if (strcmp(word,"on")==0) {			// goto.c
		sscanf(line,"%s %s %s %s",linenum,word,word2,word3);	// get goto/gosub after on
		if (strcmp(word3,"goto")==0) 		// on goto
			return run_ongoto(line);
		if (strcmp(word3,"gosub")==0) {	// on gosub
			/* get & save the line number following this line */
        	returnlinenumber[returnPos] = getNextLineNumber();
			returnPos++;
			if (returnPos > GOSUBSTACKSIZE) {
				printf("Error - return stack full\n");
				return -1;
			}
			return run_ongoto(line);
		}	
	}
	if (strcmp(word,"gosub")==0) {		// gosub.c
		/* get & save the line number following this line */
		returnlinenumber[returnPos] = getNextLineNumber();
		returnPos++;
		if (returnPos > GOSUBSTACKSIZE) {
			printf("Error - return stack full\n");
			return -1;
		}
		return run_goto(line);			// return the line number (goto/gosub functionally same)
	}
	if (strcmp(word,"return")==0) {		// return from subroutine
		returnPos--;
		if (returnPos < 0) {
			printf("Error - return without gosub\n");
			return -1;
		}
		return returnlinenumber[returnPos];
	}
	if (strcmp(word,"if")==0) {			// IF/THEN: ifthen.c
		return run_ifthen(line);
	}
	if (strcmp(word,"for")==0) {		// for/next  fornext.c
		// get & save the line # of the next line
		int i=lineptr, x=0;
		while (buffer[i++] != '\n');
		char tmpline[LINESIZE]={'\0'};
		while (1) {
			tmpline[x] = buffer[i+x];
			if (tmpline[x]=='\n') break;
			x++;
		}
		char linen[6];
		sscanf(tmpline,"%s",linen);
		forlinenumber = atoi(linen);
		return run_for(line);
	}
	if (strcmp(word,"next")==0) {		// for/next  fornext.c
		/* increment index_var by index_step. if index_var > index_end skip jump */
		if (index_var == ' ') {
			printf("Error - NEXT without FOR in line %d\n",currentlinenumber);
			exit(0);
		}
		// index var is always subscript 0
		NumVar[index_var - 'a'][0] += index_step;
		if (index_step > 0) { 
			if (NumVar[index_var - 'a'][0] > index_end) 
				return 0;
		} else {
			if (NumVar[index_var - 'a'][0] < index_end) 
					return 0;
		}
		return forlinenumber;
		
	}

	/* maybe a variable assignment w/o LET? */
	if (word[0] >= 'a' && word[0] <= 'z' && (word[1]=='(' || word[1]=='$' || word[1]=='\0' || word[1]=='=')) {
		/* re-write line to conform to LET statement */
		char TEMP[LINESIZE] = {'\0'}; int n=0;
		if (isdigit(*line)) while (isdigit(*line)) TEMP[n++] = *line++;
		if (isblank(*line)) while (isblank(*line)) TEMP[n++] = *line++;
		/* now add LET */
		TEMP[n++]='l'; TEMP[n++]='e'; TEMP[n++]='t'; TEMP[n++]=' ';
		/* and finish the line */
		while (*line != '\0') {
			TEMP[n++] = *line++;
			if (n>=LINESIZE) {
				printf("Error - runaway line in parse:assignment\n");
				return -1;
			}
		}
		/* and send the line to the LET statement */
		res = let(TEMP);
		if (error == 0) return res;
		return -1;
	}


	// unknown command
	printf("SYNTAX ERROR: [%s]\n",line);
	return -1;
}

void ignoreC(int dummy) {	// catch ctrl-c, delay for keyboard debounce
	usleep(10000);			// set flag used in runloop
	if (!RUNSTOP) return;
	RUNSTOP = 0;
	return;
}


/*************************/
/*******   MAIN   ********/
/*************************/

int main(int argc, char **argv) {

	int n=0;				// local var
	char line[LINESIZE];
	char filename[LINESIZE]={'\0'};
	int LOADFLAG = 0;
	int res = 0;
	RUNSTOP = 0;

	/* get the time in seconds, use as offset for time() function */
	BASICSTART = time(NULL);

	signal(SIGINT, ignoreC);		// ignore ctrl-C

	printf("dbasic - version %s\n",VERSION);

	/* seed the random number functions */
	srandom(time(NULL));	// will be the same if multiple runs in the same second
		
	/* set up file descriptors */
	for (int n=0; n<8; n++) fd[n]=NULL;

	/* set up the return stack (gosub's) */
	/* this is repeated in the parse loop */
	for (int n=0; n<GOSUBSTACKSIZE; n++) returnlinenumber[n] = 0;
	returnPos = 0;

	/* set up memory for the basic statements */
	buffer = malloc(BUFSIZE*sizeof(char));
    if (buffer == NULL) {
        printf("Error - buffer memory error\n");
        exit(1);
    }
	memset(buffer,0,BUFSIZE);
	LOADFLAG = 0;
	// see if filename given: if so, load it
	if (argc==1) {
		printf("Ready.\n");
		goto runloop;
	}

	// load and execute the program. 
	strcpy(filename,argv[1]);
	load(filename);
	if (pos > 0) LOADFLAG=1;
	/* start running program as soon as it's loaded */
	goto runit;
	
	

	/******************************************************************/
	/* this is the user interface when the interpreter is not running */
	/*                                                                */
	/*    commands: exit, quit, list, free, new, load, save, run      */
	/******************************************************************/

runloop:
	RUNSTOP = 0;
	fgets(line,LINESIZE,stdin);
	
	if (strncmp(line,"quit",4)==0 || strncmp(line,"exit",4)==0) {
		free(buffer);
		if (GARBAGE) {
			free(DataStorage);
			for (int n=0; n<26; n++) {
				free(NumBase[n]); NumBase[n] = NULL;
			}
		}
		exit(0);
	}
	
	if (strncmp(line,"list",4)==0) {	// show the buffer
		if (pos == 0) {
			printf("Buffer empty.\nReady.\n");
			goto runloop;
		}
		for (int i=0; i<pos; i++) printf("%c",buffer[i]);
		printf("Ready.\n");
		goto runloop;
	}
	
	if (strncmp(line,"new",3)==0) {		// clear the buffer
		memset(buffer,0,BUFSIZE);
		pos = 0;
		printf("Ready.\n");
		goto runloop;
	}
	
	if (strncmp(line,"load",4)==0) {	// load a file, run program after load
		char cmd[5]; // space for 'load' cmd
		sscanf(line,"%s %s",cmd,filename); filename[strlen(filename)]='\0';
		load(filename);
		printf("Ready.\n");
		goto runloop;
	}
	
	if (strncmp(line,"save",4)==0) {		// save the buffer to a file
		char cmd[5],filename[40];
		sscanf(line,"%s %s",cmd,filename);
		filename[strlen(filename)]='\0';
		save(filename);
		printf("Ready.\n");
		goto runloop;
	}
	
	if (strncmp(line,"run",3)==0) goto runit;
	
	if (strncmp(line,"free",4)==0) {		// show available memory
		printf("%d Bytes Free\nReady.\n",BUFSIZE-pos);
		goto runloop;
	}

	if (strncmp(line,"ver",3)==0) {
		version();
		printf("Ready.\n");
		goto runloop;
	}


	// not a command,
	// replace, delete or save the entered line
	if (line[0] == '\n') goto runloop;		// ignore empty lines
	insert(line);
	goto runloop;




	/**************************************************/
	/******** START RUNNING THE BASIC PROGRAM *********/
	/**************************************************/
runit:
	/* 
	 read each line in the buffer, parse 
	 the statements, loop until END, STOP, or buffer end 
	 is reached or an error occurs.
	 
	*/
	
	if (GARBAGE) {		// did we run before? then free up variable memory
		for (int n=0; n<26; n++) {
			free(NumBase[n]); NumBase[n] = NULL; NumVar[n] = NULL;
		}
		GARBAGE = 0;
	}

	// create/re-create space for numeric variables
	
	// set initial size so dim() doesn't have a fit when re-running
	for (int n=0; n<26; n++)
		NumSize[n] = 10;

	// set up the base vars - these hold actual values. Then assign pointers to them
	for (int n=0; n<26; n++) {
        NumBase[n] = (float *) malloc((NumSize[n]+1) * sizeof(float *));	// var(0)..var(10) 
    	if (NumBase[n] == NULL) {
			printf("Error - memory space failed for variables: base\n");
			exit(1);
		}
		NumVar[n] = NumBase[n];		// assign pointer for external functions
	}

	// let future runs know the variables exist
	GARBAGE = 1;
	// handle user breaks
	RUNSTOP = 1;

	res = 0;			// result, function return value
	char ln[LNSIZE];	// line number string placeholder
	lineptr = 0;		// initialize position in buffer
	run_clear();		// initialize user defined vars
	
	/* look for data statements, preload memory */
	res = scanData(buffer,pos);
	if (res == -1) {	// memory error - fatal
		free(buffer);
		if (GARBAGE) {
			for (int n=0; n<26; n++) {
				free(NumBase[n]);
			}
		}
		exit(1);
	}
	DataPosition = 0;	// start at beginning of data list
	if (pos == 0) {		// nothing to run!
		printf("Buffer empty.\n");
		LOADFLAG=0;
		goto runloop;
	}

	/* set up the return stack (gosub's) */
    for (int n=0; n<GOSUBSTACKSIZE; n++) returnlinenumber[n] = 0;
    returnPos = 0;


parseLoop:
	memset(line,0,LINESIZE);
	n=0;
	res = 0; error = 0;
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
	/* NOTE: line numbers are checked in insert(), but extern text files are not */
	if (currentlinenumber <= 0) {	// error
		printf("Error - bad line number %s\n",ln);
		free(buffer); free(DataStorage);
		for (int n=0; n<26;n++) free(NumBase[n]);
		/* close all open file descriptors */
        for (int n=1; n<8; n++) {
            if (fd[n] != NULL) {
                fclose(fd[n]);
                fd[n] = NULL;
            }
        }
		exit(0);
	}

	/* parse the BASIC statements, execute them */
	res = parse(line);

	/* test for eval() errors */
	if (error == -1) {
		/* close all open file descriptors */
        for (int n=1; n<8; n++) {
            if (fd[n] != NULL) {
                fclose(fd[n]);
                fd[n] = NULL;
            }
        }
		printf("Error - expression error in %d [%s]\n",currentlinenumber,line);
		LOADFLAG=0;
		printf("Ready.\n");
		goto runloop;
	}

	/* test return code */
	if (res == -998) {				// STOP statement (duplicate of END - future proof)
		/* close all open file descriptors */
        for (int n=1; n<8; n++) {
            if (fd[n] != NULL) {
                fclose(fd[n]);
                fd[n] = NULL;
            }
        }
		printf("STOPPED in line %d\n",currentlinenumber);
		if (LOADFLAG==0) {
			printf("Ready.\n");
			goto runloop;
		}
		free(buffer); free(DataStorage);
		if (GARBAGE) {
			for (int n=0; n<26; n++) {
				free(NumBase[n]);
			}
		}
		exit(0);
	}

	if (res == -999) {				// END Statement
		/* close all open file descriptors */
		for (int n=1; n<8; n++) {
			if (fd[n] != NULL) {
				fclose(fd[n]);
				fd[n] = NULL;
			}
		}
		if (LOADFLAG==0) {
			printf("Ready.\n");
			goto runloop;
		}
		free(buffer); free(DataStorage);
		if (GARBAGE) {
            for (int n=0; n<26; n++) {
                free(NumBase[n]);
            }
        }	
		exit(0);
	}
	
	if (res == -1) {				// parse returned an error
		/* close all open file descriptors */
        for (int n=1; n<8; n++) {
            if (fd[n] != NULL) {
                fclose(fd[n]);
                fd[n] = NULL;
            }
        }
		printf("Error in line %d\n",currentlinenumber);
		if (LOADFLAG==0) {
			printf("Ready.\n");
			goto runloop;
		}
		free(buffer); free(DataStorage);
		if (GARBAGE) {
            for (int n=0; n<26; n++) {
                free(NumBase[n]);
            }
        }
		exit(0);
	}
	
	if (res > 0) {					// goto/gosub statement returns new line number
		/* find the matching line number, set lineptr to start of line */
		lineptr = getstartaddress(res);
		if (lineptr == -1) {			// bad line number
			printf("Error - goto/gosub: line %d, bad line number [%d]\n",currentlinenumber,res);
			printf("Ready.\n");
			if (LOADFLAG == 0) goto runloop;
			free(buffer);
			free(DataStorage);
			if (GARBAGE) {
            	for (int n=0; n<26; n++) {
            	    free(NumBase[n]);
            	}
        	}
			exit(0);
		}
		goto parseLoop;				// continue at new address
	}

	// return code 0 is nominal return. get next line
	
	lineptr += (n+1);
	if (lineptr >= pos-1) {		// end of file reached
		printf("End of buffer reached\nReady.\n");
		LOADFLAG=0;
		goto runloop;
	}
	
	if (RUNSTOP) 
		goto parseLoop;				// continue with basic program
	
	// user hit ctrl-c
	/* close all open file descriptors */
    for (int n=1; n<8; n++) {
       if (fd[n] != NULL) {
            fclose(fd[n]);
            fd[n] = NULL;
        }
    }
	
	printf("\nBreak in %d\nReady.\n",currentlinenumber);
	goto runloop;
	
	/* all below should never be reached */
	printf("Unknown error in line %d.\n",currentlinenumber);

	/* done */
	/* close all open file descriptors */
    for (int n=1; n<8; n++) {
        if (fd[n] != NULL) {
           fclose(fd[n]);
           fd[n] = NULL;
        }
    }
	free(buffer);
	free(DataStorage);
	if (GARBAGE) {
        for (int n=0; n<26; n++) {
            free(NumBase[n]);
        }
    }
	exit(1);
}


