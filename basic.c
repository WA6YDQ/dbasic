/* basic.c - Kurt's version of Dartmuth BASIC

 Statements in this version of BASIC:
 REM, PRINT, LET, GOTO, GOSUB, RETURN, END, 
 IF/THEN, FOR/NEXT/STEP, INPUT, READ, DATA,
 RESTORE, ON/GOTO, CLEAR, STOP, DEF FNx

 Formulas: a+b, a-b, a*b, a/b, a**b, -a
 Equality Tests: >, >=, <, <=, =, <>, !=
 Logical:  a&b, a|b, a^b (and/or/xor)
 Functions: SIN(), COS(), TAN(), EXP(), LN(),
 LOG(), SQR(),

 Floating point numeric variables are a...z
 String variables are a$...z$

 All keywords, variables are in lower case.
 Case is preserved when between double quotes ""

 TODO: 
 keywords: ON/GOSUB, FOPEN(), FCLOSE(), FREAD(), 
 FWRITE(), RANDOM(IZE)
 string functions: LEFT$(), MID$(), RIGHT$()
 math functions: INT(), RND(), SGN()
 misc functions: TAB()
 arrays, DIM()

 (C) 2022 Kurt Theis <theis.kurt@gmail.com>
 This is licensed using the MIT license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dbasic.h"


/* global variables */
char *buffer;					// memory buffer holding the basic program
//int NumericVars[26];			// numeric integer variables a-z
float NumericVars[26];
char CharVars[26][LINESIZE];	// a$ thru z$, max 80 chars long each
int pos = 0;					// current end position of the buffer
int currentlinenumber = 0;		// line number of the current line being tested
int lineptr = 0;				// current buffer position during run loop
int returnlinenumber = 0;		// position in buffer when RETURN command issued
float index_start = 0;			// FOR/NEXT start index
float index_end = 0;				// FOR/NEXT end index
float index_step = 0;				// FOR/NEXT step index
char index_var;					// FOR/NEXT count variable
int forlinenumber = 0;			// FOR/NEXT line number to return to
float *DataStorage;				// DATA statement storage, will assign later
int DataStorageMax=0;			// max number of data elements
int DataPosition=0;				// position of read data objects
char fn[26][80]={};				// function definitions
int error = 0;					// used by eval(), 0 is OK, -1 is expression error

/* extern functions */
int parse(char *);
int run_let(char *);
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
/* end returns -999, stop returns -998              */
/* goto/gosubs return a line number                 */
/* -1 is an error, 0 is normal return               */
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

	if (strcmp(word,"def")==0) {		// def fnx
		res = def(line);
		return res;
	}

	if (strcmp(word,"let")==0) {		// let.c
		res = run_let(line);
		if (error == 0) return res;
		return -1;
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
		return run_goto(line);			// return the line number (goto/gosub functionally same)
	}
	if (strcmp(word,"return")==0) {		// return from subroutine
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


/*************************/
/********* MAIN **********/

int main(int argc, char **argv) {

	int n=0;				// local var
	char line[LINESIZE];
	char filename[LINESIZE]={};
	int LOADFLAG = 0;
	int res = 0;

	/* set up memory for the basic source */
	buffer = malloc(BUFSIZE*sizeof(char));
    if (buffer == NULL) {
        printf("memory error\n");
        return 1;
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
	fgets(line,LINESIZE,stdin);
	if (strncmp(line,"quit",4)==0 || strncmp(line,"exit",4)==0) {
		free(DataStorage); free(buffer);
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
	 the statements, loop until END statement 
	 is reached or an error occurs.
	 
	*/
	
	res = 0;		// result, function return value
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
	if (pos == 0) {		// nothing to run!
		printf("Buffer empty.\n");
		LOADFLAG=0;
		goto runloop;
	}

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

	/* test eval errors */
	if (error == -1) {
		printf("Error - expression error in %d [%s]\n",currentlinenumber,line);
		LOADFLAG=0;
		printf("Ready.\n");
		goto runloop;
	}

	/* test return code */
	if (res == -998) {				// STOP statement
		printf("STOPPED in line %d\n",currentlinenumber);
		printf("Ready.\n");
		if (LOADFLAG==0) goto runloop;
		free(buffer); free(DataStorage);
		exit(0);
	}
	if (res == -999) {				// END Statement
		printf("END statement in line %d\n",currentlinenumber);
		printf("Ready.\n");
		if (LOADFLAG==0) goto runloop;
		free(buffer); free(DataStorage);
		exit(0);
	}
	if (res == -1) {				// parse returned an error
		printf("Error in line %d\n",currentlinenumber);
		free(buffer);
		free(DataStorage);
		if (LOADFLAG==0) goto runloop;
		free(buffer); free(DataStorage);
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
		printf("End of buffer reached\nReady.\n");
		LOADFLAG=0;
		goto runloop;
	}
	goto parseLoop;				// continue with basic program

	
	/* all below should never be reached */
	printf("Unknown error in line %d.\n",currentlinenumber);

	/* done */
	free(buffer);
	free(DataStorage);
	return 0;
}


