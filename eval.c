/* eval.c - evaluate an expression, return value
 * math expressions: + - * / **
 * logic expressions: & | ^
 *
 * TODO ~  a[n]  a[101]
 */

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isnumber(char ch) {		// return 1 if char is 0-9, ., e, E, -
	if (ch >='0' && ch <='9') return 1;
	if (ch == 'e' || ch == 'E') return 1;
	if (ch == '.' || ch == '-') return 1;
	return 0;
}


float eval(char *expr) {

	char numstr[16] = {};
	float lvalue =0.0, rvalue = 0.0, Plvalue = 0.0;
	extern float NumericVars[];
	int FLAG=1;
	int MATHFLAG=0; int PMATHFLAG=0;
	char MATHFUNC=' '; char PMATHFUNC=' ';
	float doArith(float,float,char);
	extern char fn[26][80];

	//printf("eval: line=[%s]\n",expr);
	MATHFLAG = 0;
	
	while (1) {				// test all chars in expr
		memset(numstr,0,sizeof(numstr));
		int n=0;

		/* end of line? (should only see \0 as end of expr char) */
		if (*expr == '\0' || *expr == ',' || *expr == ';') return lvalue;

		/* test space (no op) */
		if (isblank(*expr)) {
			expr++;
			continue;
		}

		/* test unary - (negate) for numbers */
		if (*expr == '-' && (isdigit(*(expr+1)) || *(expr+1)=='.') && FLAG) {		// 1st char of expression when FLAG
			//printf("in unary:expr=[%s]\n",expr);
			expr++;		// point past -
			while (1) {
				if (!isdigit(*expr) && *expr != '.') break;
				numstr[n] = *expr;
				expr++; n++;
			}
			lvalue = -1 * atof(numstr);
			FLAG = 0;
			continue;
		}

		/* test unary - (negate) for variables */
		if (*expr == '-' && isalpha(*(expr+1)) && FLAG) {
			expr++;
			lvalue = -1 * NumericVars[(*expr)-'a'];
			FLAG = 0;
			expr++;		// point past variable
			continue;
		}
		
		/* test defined functions (fn[a..z]) */
		if (*expr == 'f' && *(expr+1) == 'n') {
			char fnchar = *(expr+2);
			if (fnchar < 'a' || fnchar > 'z') {	
				printf("Error - bad function name fn%c\n",fnchar);
				return -1;
			}
			char *tmpfn = fn[fnchar-'a'];
			float funcvalue = eval(tmpfn);
			expr += 3;		// skip fnx
			if (FLAG) {
                lvalue = funcvalue;
				FLAG=0;
            } else {
                rvalue = funcvalue;
			}
			continue;
		}


		/* test functions: sin(x+25)+cos(a) */
		if (isalpha(*expr) && isalpha(*(expr+1))) {
			char functname[8] = {}; int n=0;
			while (*expr != '(') {
				functname[n] = *expr;
				n++; expr++;
			}
			expr++;		// skip (
			// expr points to (
			char functval[40]={}; n=0;
			while (*expr != ')') {
				functval[n] = *expr;
				n++; expr++;
			}
			expr++;		// skip )
			float tvalue = eval(functval);
			float funcvalue;

			/* now determine function, and run it against tvalue */
			/*   SIN()   */
			if (strncmp(functname,"sin",3)==0) {
				funcvalue = sinf(tvalue);
				goto functend;
			}
			/*   COS()   */
			if (strncmp(functname,"cos",3)==0) {
				funcvalue = cosf(tvalue);
				goto functend;
			}
			/*   TAN()   */
			if (strncmp(functname,"tan",3)==0) {
                funcvalue = tanf(tvalue);
                goto functend;
            }
			/*   EXP()   */
			if (strncmp(functname,"exp",3)==0) {
				funcvalue = expf(tvalue);
				goto functend;
			}
			/*   LOG() (natural log) */
			if (strncmp(functname,"log",3)==0) {
				funcvalue = logf(tvalue);
				goto functend;
			}
			/*   LOG10() (base 10 log)  */
			if (strncmp(functname,"log10",5)==0) {
				funcvalue = log10f(tvalue);
				goto functend;
			}
			/*   SQR()    */
			if (strncmp(functname,"sqr",3)==0) {
				funcvalue = sqrtf(tvalue);
				goto functend;
			}
			
			printf("Error - undefined function %s()\n",functname);
			return -1;

functend:	/* add/etc function value */
			if (FLAG) {
				lvalue = funcvalue;
				FLAG=0;
			} else
				rvalue = funcvalue;
			/* and test for math */
			if (MATHFLAG) {
				lvalue = doArith(rvalue,lvalue,MATHFUNC);
				rvalue = 0;
				MATHFUNC = ' '; MATHFLAG=0;
				continue;
			}
			continue;
		}


		/* test numbers */
		if (isnumber(*expr)) {
			while (1) {
				if (*expr == '\0' || *expr == '\n') break;
				if (!isnumber(*expr)) break;
				numstr[n] = *expr;
				expr++; n++;
			}
			if (FLAG) {
				lvalue = atof(numstr);
				FLAG=0;
			} else
				rvalue = atof(numstr);
			//printf("before lvalue=%d %c rvalue=%d\n",lvalue,MATHFUNC,rvalue);
			if (MATHFLAG) {
				lvalue = doArith(rvalue,lvalue,MATHFUNC);
				rvalue = 0;
				MATHFUNC = ' '; MATHFLAG=0;
			//printf("eval:numeric value, lvalue=%f\n",lvalue);
				//printf("after lvalue=%d  %c  rvalue=%d\n",lvalue,MATHFUNC,rvalue);
				continue;
			}
			continue;
		}

		/* test numeric variables a-z */
		if (isalpha(*expr)) {		// test single letter numeric vars
			if (FLAG) {
				lvalue = NumericVars[(*expr)-'a'];
				FLAG=0;
			} else
				rvalue = NumericVars[(*expr)-'a'];
			if (MATHFLAG) {
				lvalue = doArith(rvalue,lvalue,MATHFUNC);
				rvalue = 0;
				MATHFUNC=' '; MATHFLAG=0;
			//printf("eval:numeric vars, lvalue=%f\n",lvalue);
				continue;
			}
			expr++;
			continue;
		}
		
		/* test math functs */
		if (*expr == '+' || *expr == '-' || *expr == '*' || *expr == '/') {
			MATHFLAG = 1; MATHFUNC = *expr;
			if (*expr == '*' && *(expr+1) == '*') {		// exponientation, 2**8
				MATHFUNC = 'E';
				expr++;
			}
			expr++;
			continue;
		}

		/* test logic functs */
		if (*expr == '&' || *expr == '|' || *expr == '^') {
			MATHFLAG = 1; MATHFUNC = *expr;
			expr++;
			continue;
		}


		/* test parens */
		if (*expr == '(') {		// start of enclosed function
			Plvalue = lvalue; lvalue = 0; FLAG=1;
			if (MATHFLAG) {
				PMATHFLAG = MATHFLAG; PMATHFUNC = MATHFUNC;
				MATHFLAG=0; MATHFUNC=' ';
			}
			expr++;
			continue;
		}
		if (*expr == ')') {		// end of enclosed function
			MATHFLAG = PMATHFLAG; MATHFUNC = PMATHFUNC;
			if (MATHFLAG) {
				lvalue = doArith(lvalue,Plvalue,MATHFUNC);
				Plvalue = 0; rvalue=0; MATHFUNC = ' '; MATHFLAG=0; FLAG=0;
			}
			expr++;
			continue;
		}


		printf("unknown char[%c]\n",*expr);
		expr++;				// point to next
		continue;
	}


}

float doArith(float rvalue,float lvalue,char MATHFUNC) {
	//printf("(doArith: lval[%d]  rval[%d]  )",lvalue,rvalue);
	if (MATHFUNC == '+') return (lvalue += rvalue);
	if (MATHFUNC == '-') return (lvalue -= rvalue);
	if (MATHFUNC == '*') return (lvalue *= rvalue);
	if (MATHFUNC == '/') return (lvalue /= rvalue);
	if (MATHFUNC == 'E') return (lvalue = pow(lvalue,rvalue));
	if (MATHFUNC == '&') return ((int)lvalue & (int)rvalue);
	if (MATHFUNC == '|') return ((int)lvalue | (int)rvalue);
	if (MATHFUNC == '^') return ((int)lvalue ^ (int)rvalue);

	printf("Unknown expression %c in eval routine\n",MATHFUNC);
	return 0;
}
