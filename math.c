/* 
 * math routines and eval 
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
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define STACKMAX 20

/* globals for this set of routines */


/* return 1 if char is a number */
int isnum(char ch) {
	if (ch >= '0' && ch <= '9') return 1;
	if (ch == 'e' || ch == 'E') return 1;
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

	extern float NumericVars[];
	int NEGFLAG=0;		// true when unary - found
	//int MATHFLAG=0;		// true when pending math operation

	float val[STACKMAX];		// holds object values
	int valpos = 1;
	char mathchr[STACKMAX];		// holds math expressions
	int mathchrpos = 1;

	/* initialize storage */
	for (int n=0; n<STACKMAX; n++) val[n]=0;
	for (int n=0; n<STACKMAX; n++) mathchr[n]='\0';

	//printf("math: received expr:[%s]\n",expr);

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

		/* see if expression is enclosed by parenthesis: group them and eval */
		/* NOTE: DOES NOT WORK FOR NEXTED () */
		if (*expr == '(') {
			expr++;			// skip opening parens
			int n=0; char temp[80]={}; memset(temp,0,sizeof(temp));
			while (1) {
				if (*expr == ')') break;
				temp[n] = *expr;
				expr++; n++;
			}
			expr++;		// skip ending parens
			float res = eval(temp);		// evaluate the expression between ()
			val[valpos] = res;
			valpos++;
			continue;
		}

		/* test for math function (sin, tan, sqr, etc.) */
		if (isalpha(*expr) && isalpha(*(expr+1))) {
			char funcname[10]={}; int n=0;
			while (*expr != '(') {
				funcname[n] = *expr;
				expr++; n++;
			}
			expr++;		// skip initial (
			// now get value inside ()
			char funcval[40]={}; n=0;
			while (*expr != ')') {
				funcval[n] = *expr;
				expr++; n++;
			}
			expr++;		// skip ending )
			float tvalue = eval(funcval);
			float fvalue = 0.0;
			/* now see what the function is */
			/* SQR() */
			if (strncmp(funcname,"sqr",3)==0) {
				fvalue = sqrt(tvalue);
				goto funcend;
			}
			/* SIN() */
			if (strncmp(funcname,"sin",3)==0) {
				fvalue = sinf(tvalue);
				goto funcend;
			}
			/* COS()  */
			if (strncmp(funcname,"cos",3)==0) {
				fvalue = cosf(tvalue);
				goto funcend;
			}
			/* TAN()  */
			if (strncmp(funcname,"tan",3)==0) {
				fvalue = tanf(tvalue);
				goto funcend;
			}
			// not a recognized function
			printf("Error - %s not a recognized function\n",funcname);
			exit(1);
			// put function result in list
			funcend:
			val[valpos] = fvalue;
			valpos++;
			continue;
		}

		/* at EOL break from this loop and calculate expression */
		if (*expr == '\0' || *expr == '\n') break;

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


		/* save arithmetic chars in their own set of variables */
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




		/* convert single char variables into numbers */
		if (*expr >= 'a' && *expr <= 'z' && *(expr+1)!='$') {
			float res = NumericVars[*expr - 'a'];
			if (NEGFLAG) {
				res = res * -1;
				NEGFLAG=0;
			}
			val[valpos] = res;
			expr++; valpos++;
			continue;
		}

		/* combine digits (including e/E and .) into a number */
		if (isnum(*expr)) {
			int n=0; char number[12]={};
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

		/* undefined character in expression */	
		printf("math: unknown char '%c'\n",*expr);
		expr += 1;
		continue;

	

	}

	/* EOL reached - parse the resultant sequence of numbers & expressions */
	
	/*
	printf("\nparsing: ");
	for (int x=1; x<valpos; x++)
		printf("%g%c",val[x],mathchr[x]);
	printf("\n");
	*/

	int i=0, FLAG=0; float res=0; int n=0;

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
