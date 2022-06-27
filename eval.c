/* 
 * math routines and eval for strings 
 * (c) k theis 2022 <theis.kurt@gmail.com>
 *
 * This is the math formula, expression and number
 * evaluation routine. Given a single number, variable, 
 * simple expression or math function or user defined
 * function, parse and return the value.
 *
 * Order of operation is algabraic in this order:
 * (), - (unary minus), ** (x**y), *, /, +, -, 
 * & (bitwise AND), | (bitwise OR), ^ (bitwise XOR)
 *
 * Formulas are evaluated as they appear in the expression.
 * All evaluations are float. Where expressions need to
 * be integer, they are converted before operation.
 *
 * this is part of dbasic
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "dbasic.h"

#define STACKMAX 20



/* return 1 if char is a number */
int isnum(char ch) {
	if (ch >= '0' && ch <= '9') return 1;
	if (ch == 'e' || ch == '-') return 1;
	if (ch == '.') return 1;
	return 0;
}

/* return 1 if char is math/logic function */
int ismathchar(char ch) {
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/') return 1;
	if (ch == '&' || ch == '|' || ch == '^') return 1;
	if (ch == 'E') return 1;		// E is used for **
	return 0;
}


float domath(float lval, float rval, char ch) {
	// debugging
	// printf("domath: lval=%g  rval=%g  func=%c\n",lval,rval,ch);
	if (ch == '+') return lval+rval;
	if (ch == '-') return lval-rval;
	if (ch == '*') return lval*rval;
	if (ch == '/') return lval/rval;
	if (ch == 'E') return pow(lval,rval);	// a**x
	// logic operators
	if (ch == '&') return ((int)lval & (int)rval);
	if (ch == '|') return ((int)lval | (int)rval);
	if (ch == '^') return ((int)lval ^ (int)rval);
	// ignore other operators
	return NAN;
}



/****** EVAL *******/
/* return float value of supplied expression */
float eval(char *expr) {

	extern long int BASICSTART;
	extern float *NumVar[26];
	extern char fn[26][80];
	extern char CharVars[26][LINESIZE];
	extern int NumSize[26];
	extern int fdnumber;
	extern FILE *fd[8];

	int NEGFLAG=0;		// true when unary - found

	float val[STACKMAX]={0};		// holds object values
	int valpos = 1;
	char mathchr[STACKMAX]={'\0'};		// holds math expressions
	int mathchrpos = 1;

	char funcname[20] = {'\0'};
	char funcval[40] = {'\0'};

	extern int error;		// 0 if OK, -1 on error. 

	/* initialize storage */
	for (int n=0; n<STACKMAX; n++) val[n]=0;
	for (int n=0; n<STACKMAX; n++) mathchr[n]='\0';

	// debugging
	// printf("eval: expression: [%s]\n",expr);

	error = 0;		// initialize on entry

	while (1) {

		/* 
		 * This is the main expression loop. 
		 * Step thru each character. Convert 
		 * numeric variables into numbers, functions
		 * into numbers, and strings of digits
		 * into numbers then save each number 
		 * into a variable (val[x]). Save each
		 * arithmetic character (+,-,*,/,**) in
		 * it's own variable (mathchr[x]). When 
		 * EOL is reached, parse the resultant 
		 * string of var[x] & mathchr[x] algabraically
		 * into a single numeric result in val[1].
		 */


		if (isblank(*expr)) {		// ignore spaces/tabs
			expr++;
			continue;
		}

		/* test for user-defined functions (fn[a..z]) */
		if (strlen(expr) >= 3) {
			if (*expr == 'f' && *(expr+1) == 'n' && (*(expr+2) >= 'a' && *(expr+2) <= 'z')) {
				char fnchar = *(expr+2);
				char *tmpfn = fn[fnchar - 'a'];
				expr += 3;
				float res = eval(tmpfn);
				val[valpos] = res;
				valpos++;
				continue;
			}
		}


		/* test for math function (sin, tan, sqr, etc.) */
		if (strlen(expr) >= 5) {
			if (isalpha(*expr) && isalpha(*(expr+1))) {
				int n=0; int pc=0;	// parens count for functions
				memset(funcname,0,sizeof(funcname));
				// get the name of the function up to ( 
				while (1) {
					if (*expr == '(') break;
					funcname[n] = *expr;
					expr++; n++;
				}
				pc = 1;		// allow for nested function calls
				expr++;		// skip initial (
				// now get value inside ()
				n=0;
				memset(funcval,0,sizeof(funcval));
				while (1) {
					if (n > sizeof(funcval)) break;
					if (*expr == '(') {	 // nested function
						pc++;
					}
					if (*expr == ')') {
						pc--;
						if (pc == 0) break;
					}
					funcval[n] = *expr;
					expr++; n++;
				}
				expr++;		// skip ending )
				float fvalue = 0;
				/* now see what the function is */
				/* SQR()  square root */
				if (strncmp(funcname,"sqr",3)==0) {
					float tvalue = eval(funcval);
					fvalue = sqrtf(tvalue);
					goto funcend;
				}
				/* NOTE: All trig functions are in radians */
				/* SIN() sin x where value is in radians */
				if (strncmp(funcname,"sin",3)==0) {
					float tvalue = eval(funcval);
					fvalue = sinf(tvalue);
					goto funcend;
				}
				/* COS()  cos x where valie is in radians */
				if (strncmp(funcname,"cos",3)==0) {
					float tvalue = eval(funcval);
					fvalue = cosf(tvalue);
					goto funcend;
				}
				/* TAN()  tan x where value is in radians */
				if (strncmp(funcname,"tan",3)==0) {
					float tvalue = eval(funcval);
					fvalue = tanf(tvalue);
					goto funcend;
				}
				/* ATN() - arctan (atanf in c) where value is in radians */
				if (strncmp(funcname,"atn",3)==0) {
					float tvalue = eval(funcval);
					fvalue = atanf(tvalue);
					goto funcend;
				}
				/* EXP()  */
				if (strncmp(funcname,"exp",3)==0) {
					float tvalue = eval(funcval);
					fvalue = expf(tvalue);
					goto funcend;
				}
				/* LOG()  (base 10) log(42)=1.6232 */
				if (strncmp(funcname,"log",3)==0) {
					float tvalue = eval(funcval);
					fvalue = log10f(tvalue);
					goto funcend;
				}
				/* LN() natural log, ln(42)=3.7377 */
				if (strncmp(funcname,"ln",2)==0) {
					float tvalue = eval(funcval);
					fvalue = logf(tvalue);
					goto funcend;
				}
				/* INT() return int() of expression */
				if (strncmp(funcname,"int",3)==0) {
					float tvalue = eval(funcval);
					fvalue = (int)tvalue;
					goto funcend;
				}
				/* ABS() return absolute value of expression */
				if (strncmp(funcname,"abs",3)==0) {
					float tvalue = eval(funcval);
					fvalue = abs(tvalue);
					goto funcend;
				}
				/* SGN() - return -1, 0, 1 for negative, zero, positive values */
				if (strncmp(funcname,"sgn",3)==0) {
					float tvalue = eval(funcval);
					if (tvalue < 0) fvalue = (int)-1;
					if (tvalue == 0) fvalue = 0;
					if (tvalue > 0) fvalue = (int)1;
					goto funcend;
				}
				/* ASC() return ascii number of supplied character */
				if (strncmp(funcname,"asc",3)==0) {
					if (funcval[0]=='"' && funcval[2]=='"') {
						fvalue =  (char)funcval[1];
						goto funcend;
					}
					if (funcval[0] >= 'a' && funcval[0] <= 'z' && funcval[1] == '$') {
						fvalue = (char)CharVars[funcval[0]-'a'][0];
						goto funcend;
					}
					fvalue = 0;
					goto funcend;
				}
				/* LEN() return length of string in string variable (a$) */
				if (strncmp(funcname,"len",3)==0) {
					if (funcval[0] >= 'a' && funcval[0] <= 'z' && funcval[1] == '$') {
						fvalue = strlen(CharVars[funcval[0]-'a']);
						goto funcend;
					}
					fvalue = 0;
					goto funcend;
				}
				/* RND() return random number between 0 and 1 */
				if (strncmp(funcname,"rnd",3)==0) {
					/* note: srandom is seeded in basic.c at start */
					/* rnd(-1): reseed with a random number. */
					/* rnd(0) or rnd(): no reseeding, return a random number */
					/* rnd(1): reseed with a fixed value for repeatable results */
					int seed = (int)eval(funcval);  // returns 0, nothing between ()
					if (seed > 0) 
						srandom(seed);		// fixed seed value
					else if (seed == -1)
						srandom(time(NULL)+random());
					fvalue = (random() % 100000);
					fvalue = fvalue / 100000.0;
					goto funcend;
				}
				/* DEG() convert radian # to degree */
				if (strncmp(funcname,"deg",3)==0) {
					float tvalue = eval(funcval);
					fvalue = tvalue * 180.0/3.141592654;
					goto funcend;
				}
				/* RAD() convert degrees to radians */
				if (strncmp(funcname,"rad",3)==0) {
					float tvalue = eval(funcval);
					fvalue = tvalue * 3.141592654/180.0;
					goto funcend;
				}
				/* PI() return pi */
				if (strncmp(funcname,"pi",2)==0) {
					fvalue = 3.141592654;
					goto funcend;
				}
				/* VAL() - return the numeric value of the given string */
				/* ex: val("2*x+(3*pi(0)/sin(a))-4")    */
				/* let a$="1/34*75+sin(8)" : let a=val(a$) */
				/* val(2*a+b/c) */
				if (strncmp(funcname,"val",3)==0) {
					/* test for a string variable */
					if (funcval[0] >= 'a' && funcval[0] <= 'z' && funcval[1] == '$') {
						fvalue = eval(CharVars[funcval[0]-'a']);
						goto funcend;
					}
					/* test for a quoted string */
					if (funcval[0] == '"') {
						char temp[LINESIZE]={'\0'}; int n=0;
						while (1) {
							if (funcval[n+1] == '"') break;
							temp[n] = funcval[n+1];
							n++;
						}
						temp[n] = '\0';
						fvalue = eval(temp);
						goto funcend;
					}
					/* try all between () */
					fvalue = eval(funcval);
					goto funcend;
				}

				/* EOF() - test if file number reached EOF() */
				if (strncmp(funcname,"eof",3)==0) {
					if (funcval[0] != '#') {
						printf("Error - bad format in eof() for file number\b");
						error = -1;
						return NAN;
					}
					fdnumber = atoi(&funcval[1]);
					if (fd[fdnumber] == NULL) {
						printf("Error - file %d is not open\n",fdnumber);
						error = -1;
						return NAN;
					}
					if (feof(fd[fdnumber])) 
						fvalue = 1;
					else
						fvalue = 0;
					goto funcend;
				}

				/* TIME() return number of seconds since the interpreter was started */
				/* NOTE: this requires the unix time() library. If your machine doesn't
				 * have one, substitute your own or comment this out */
				
				if (strncmp(funcname,"time",4)==0) {
					time_t tres = time(NULL);
					fvalue = (long int)tres - BASICSTART;		// found in basic.c main()
					goto funcend;
				}

				// not a recognized function
				printf("Error - unknown numeric function %s\n",funcname);
				error = -1;
				return NAN;
		
funcend:		// put function result in list for later
				val[valpos] = fvalue;
				valpos++;
				continue;
			}
		}

		/* at EOL break from this loop and calculate expression */
		if (*expr == '\0' || *expr == '\n') {
			break;
		}

		/* test unary - */
		/* NOTE: to assign a negitive value the unary - must
		 * be the first character of the expression ie -1*10*b
		 * this includes inside parens ie (-1*5)
		 */
		if (*expr == '-' && valpos == 1) {
			NEGFLAG = 1;
			expr++;
			continue;
		}


		/* save arithmetic operands in their own set of variables */
		if (ismathchar(*expr)) {
			if (*expr == '*' && *(expr+1) == '*') {
                mathchr[mathchrpos++] = 'E';
                expr += 2;
            } else {
                mathchr[mathchrpos++] = *expr;
                expr++; 
            }
            continue;
        }




		/* convert numeric variables into numbers */
		if (*expr >= 'a' && *expr <= 'z' && *(expr+1)!='$') {
			/* get subscript */
			int subscript = 0; char charvar = *expr;
			expr++;
			if (*expr == '(') {		// using subscript
				expr++;				// point past (
				char SUBNUM[12]={'\0'}; int n=0;
				while (1) {
					if (*expr == ')') break;
					SUBNUM[n] = *expr;
					expr++; n++;
				}
				subscript = (int)eval(SUBNUM);
				expr++;
			}
			// error bounds checking
			if (subscript < 0 || subscript > NumSize[charvar-'a']) {
				printf("Error - bounds error in subscript: %c(%d)\n",charvar,subscript);
				error = -1;
				return NAN;
			}

			float res = NumVar[charvar-'a'][subscript];
			if (NEGFLAG) {
				res = res * -1;
				NEGFLAG=0;
			}
			val[valpos] = res;
			valpos++;
			continue;
		}

		/* combine digits (including e/E and .) into a number */
		if (isnum(*expr)) {
			int n=0; char number[20];
			memset(number,0,sizeof(number));
			while (1) {
				if (isnum(*expr)==0) break;
				number[n] = *expr;
				n++; expr++;
			}
			float res = atof(number);
			if (NEGFLAG) {
				res = res * -1;
				NEGFLAG=0;
			}
			val[valpos] = res;
			valpos++;
			continue;
		}

		/* see if expression is enclosed by parenthesis: group them and eval */
        if (*expr == '(') {
			expr++;         // skip opening parens
            int n=0; char temp[80]={'\0'}; 
			memset(temp,0,sizeof(temp));
            int parencount=1;
			while (1) {
                if (*expr == ')') parencount--;
				if (parencount==0) break;
                if (*expr == '(') parencount++;
                temp[n] = *expr;
                expr++; n++;
            }
            expr++;     // skip ending parens
			float res = eval(temp);     // evaluate the expression between ()
			val[valpos] = res;
            valpos++;
            continue;
        }


		/* undefined character in expression */	
		printf("eval: unknown char '%c' in expression.\n",*expr);
		error = -1;
		return NAN;
	

	}

	/* EOL reached - parse the resultant sequence of numbers & expressions */
	

	int i=0, FLAG=0; float res=0; int n=0;


	/* error checking */
	if (mathchrpos == 1 && valpos > 2) {
		error = -1;
		return NAN;
	}



	/* order of precidence for arithmetic operators */
	char oper[] = {'E','E','*','/','+','-','&','|','^','^'};


	/* test each expression with operators in order from above */
	for (n=0; n<10; n+=2) {
		while (1) {
    	    FLAG=0;
    	    for (i=1; i<valpos; i++) {
    	        if (mathchr[i] == oper[n] || mathchr[i] == oper[n+1]) {
    	            res = domath(val[i],val[i+1],mathchr[i]);
    	            val[i] = res;
    	            // shift down rest
    	            for (int x=i+1; x<valpos; x++) {
    	                val[x] = val[x+1];
    	                mathchr[x-1] = mathchr[x];
    	            }
    	            FLAG=1;
    	        }
    	    }
    	    if (FLAG) continue;
    	    break;
    	}
	}
	
	return val[1];		// result

	/* end of eval() */
}


/* ****************************** */
/* *****     EVALSTRING()   ***** */
/*   evaluate string expressions  */
/* ****************************** */

char *evalstring(char *line) {

	extern char tempCharVar[MAXLINESIZE];
	extern char CharVars[26][LINESIZE];
	extern float *NumVar[26];
	extern int error;
	error = 0;
	char chrvar = '\0';
	char temp[LINESIZE]={'\0'};
	int n=0;

	memset(tempCharVar,0,MAXLINESIZE);
evalstrLoop:

	if (*line == '\0' || *line == '\n')     // assignment complete
        return tempCharVar;

	/* ignore spaces outside quotes */
	if (*line == ' ') {
		line++;
		goto evalstrLoop;
	}

	/* test for single letter variables */
	if (*line >= 'a' && *line <= 'z' && *(line+1) == '$') {
		chrvar = *line; line += 2;
		strcat(tempCharVar,CharVars[chrvar-'a']);
		goto evalstrLoop;
	}
	
	if (*line == '+') {		// catenation
		line++;
		goto evalstrLoop;
	}

	if (*line == '"') {		// get everything between " "
		line++;				// skip "
		while (1) {
			if (*line == '"') break;
			strncat(tempCharVar,line,1);
			line++;
		}
		line++;		// skip "
		goto evalstrLoop;
	}


	/* test if functions */
	n=0; memset(temp,0,sizeof(temp));
	while (1) {
		if (*line == '(') break;	// get function until (
		temp[n] = *line;
		n++; line++;
		if (n > LINESIZE) {
			printf("Error - excessive characters in string\n");
			error = -1;
			return "";
		}
	}

	/* ---------------------------------------------- */

	/* LEFT$(X$,C) */
	if (strncmp(temp,"left$",5)==0) {
		// ie. left$(a$,3)
		line++;		// skip (
		if (!(*line >= 'a' && *line <= 'z' && *(line+1) == '$')) {
			printf("Error - expected character variable in left$()\n");
				error = -1;
				return "";
		}
		char strchr = *line;
		line += 2;	// skip x$
		if (*line != ',') {
			printf("Error - expected seperator char in left$(). Got %c\n",*line);
			error = -1;
			return "";
		}
		line++;		// skip ,
		char tmpnum[LINESIZE]={'\0'}; n=0;
		while (1) {
			if (*line == ')') break;	// get number between ,)
			if (*line == '\0') {
				printf("Error - bad expression in left$()\n");
				error = -1;
				return "";
			}
			tmpnum[n] = *line;
			n++; line++;
		}
		line++;		// skip )
		int val = eval(tmpnum);
		if (val == 0) 
			return "";
		// get chars from 0 thru tmpnum
		n=0;
		while (n < val) {
			strncat(tempCharVar,&CharVars[strchr-'a'][n],1);
			n++;
		}
		goto evalstrLoop;
	}

	/* --------------------------------------------- */

	/* RIGHT$(X$,C) */
	if (strncmp(temp,"right$",6)==0) {
		// ie. right$(a$,3)
        line++;     // skip (
        if (!(*line >= 'a' && *line <= 'z' && *(line+1) == '$')) {
            printf("Error - expected character variable in right$()\n");
                error = -1;
                return "";
        }
        char strchr = *line;
        line += 2;  // skip x$
        if (*line != ',') {
            printf("Error - expected seperator char in right$(). Got %c\n",*line);
            error = -1; 
            return ""; 
        }
        line++;     // skip ,
        char tmpnum[LINESIZE]={'\0'}; n=0;
        while (1) {
            if (*line == ')') break;    // get number between ,)
            if (*line == '\0') {
                printf("Error - bad expression in right$()\n");
                error = -1; 
                return ""; 
            }
            tmpnum[n] = *line;
            n++; line++;
		}
		line++;		// skip )
        int val = eval(tmpnum);
        if (val == 0)  
            return ""; 
		// get the last (val) chars of x$
        n=strlen(CharVars[strchr-'a']); 
		val=n-val;
        while (val < n) {
			strncat(tempCharVar,&CharVars[strchr-'a'][val++],1);
        }
		goto evalstrLoop;
    }

	/* --------------------------------------------- */

	/* MID$(X$,S,C)   X$: string, S:start position, C:count */
	if (strncmp(temp,"mid$",4)==0) {
		// ie. mid$(a$,3,2)		starting at pos 3 for 2 chars
		int startnum,countnum;
		char tmpnum[LINESIZE];
        line++;     // skip (
        if (!(*line >= 'a' && *line <= 'z' && *(line+1) == '$')) {
            printf("Error - expected character variable in mid$()\n");
                error = -1;
                return "";
        }
        char strchr = *line;
        line += 2;  // skip x$
        if (*line != ',') {
            printf("Error - expected seperator char in mid$(). Got %c\n",*line);
            error = -1;
            return "";
        }
        line++;     // skip ,
        // get startnum
		memset(tmpnum,0,LINESIZE); n=0;
        while (1) {
            if (*line == ',') break;    // get number between ,)
            if (*line == '\0') {
                printf("Error - bad expression in mid$(): startchar\n");
                error = -1;
                return "";
            }
            tmpnum[n] = *line;
            n++; line++;
        }
        startnum = eval(tmpnum); startnum -= 1;	// really starts at 0
		line++;		// skip ,
		// get countnum
		memset(tmpnum,0,LINESIZE); n=0;
		while (1) {
            if (*line == ')') break;    // get number between ,)
            if (*line == '\0') {
                printf("Error - bad expression in mid$(): countchar\n");
                error = -1;
                return "";
            }
            tmpnum[n] = *line;
            n++; line++;
        }
		line++;		// skip )
		countnum = eval(tmpnum);
        if (startnum < 0 || countnum == 0) return "";
        // get start pos, count number of chars of x$
		if (startnum > strlen(CharVars[strchr-'a'])) return "";
		if (startnum+countnum > strlen(CharVars[strchr-'a'])) return "";
		for (int x=startnum; x<(startnum+countnum); x++) 
				strncat(tempCharVar,&CharVars[strchr-'a'][x],1);
        goto evalstrLoop;	
    }

	/* ------------------------------------------- */


	/* chr$() return char based on value: v$=chr$(65) */
	if (strncmp(temp,"chr$",4)==0) {
		line++;		// skip (
		
		// test if variable a-z
		if (*line >= 'a' && *line <= 'z') {		// numeric var
			int subscript = 0;
			char charvar = *line;
			line++;
			if (*line == '(') {	// get subscript
				line++;		// skip (
				char SUBNUM[LINESIZE]={'\0'};
				int n=0;
				while (1) {		// get subscript
					if (*line == ')') break;
					SUBNUM[n] = *line;
					line++; n++;
				}
				subscript = eval(SUBNUM);
				line++;
			}
			line++; 	// skip )
			char ch = (char)NumVar[charvar-'a'][subscript];
			strncat(tempCharVar,&ch,1);
			goto evalstrLoop;
		}
		
		// test if number
		char tmpnum[LINESIZE];
		memset(tmpnum,0,LINESIZE);
		n=0; 
		while (1) {
			if (!isdigit(*line)) break;
			tmpnum[n] = *line;
			n++; line++;
		}
		line++;		// skip )
		n = eval(tmpnum); char ch=n;
		strncat(tempCharVar,&ch,1);
		goto evalstrLoop;
	}


	/* str$() - convert numeric to string */
	if (strncmp(temp,"str$",4)==0) {
		line++;			// skip (

		/* get all between () and eval() */
		char TEMP[LINESIZE]={'\0'}; int n=0; int pc=0;
		while (1) {
			if (*line == '(') {
				TEMP[n] = *line;
				pc++; line++; n++;
				continue;
			}
			if (*line == ')' && pc == 0) break;
			if (*line == ')') pc--;
			if (*line == '\n') {
				printf("Error - early end of function str$\n");
				error = -1;
				return "";
			}
			TEMP[n++] = *line++;
		}
		line++;		// skip )
		float f = eval(TEMP);
		/* convert number to string */
		char tmp[LINESIZE] = {'\0'};
		sprintf(tmp,"%g",f);
		strcat(tempCharVar,tmp);
		goto evalstrLoop;
	}


		

	/* --------------------------------------------- */

	printf("Error - Unknown character function in string eval()\n");
	error = -1;
	return "";
}
