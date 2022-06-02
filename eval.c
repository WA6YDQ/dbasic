/* eval.c - evaluate an expression, return value
 * math expressions: + - * / **
 * logic expressions: & | ^
 *
 * TODO ~  a$  a[n]  a[101]
 */

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int eval(char *expr) {

	char numstr[16] = {};
	int lvalue=0, rvalue=0; int Plvalue=0;
	extern int NumericVars[];
	int FLAG=1;
	int MATHFLAG=0; int PMATHFLAG=0;
	char MATHFUNC=' '; char PMATHFUNC=' ';
	int doArith(int,int,char);

	//printf("eval: [%s]\n",expr);
	MATHFLAG = 0;
	
	while (1) {				// test all chars in expr
		memset(numstr,0,sizeof(numstr));
		int n=0;

		/* end of line? (should only see \0 as end of expr char) */
		if (*expr == '\0' || *expr == ',' || *expr == ';') return lvalue;

		/* test space (no op) */
		if (*expr == ' ') {
			expr++;
			continue;
		}

		/* test unary - (negate) for numeric values */
		if (*expr == '-' && isdigit(*(expr+1)) && FLAG) {		// 1st char of expression when FLAG
			//printf("in unary:expr=[%s]\n",expr);
			expr++;		// point past -
			while (1) {
				if (!isdigit(*expr)) break;
				numstr[n] = *expr;
				expr++; n++;
			}
			lvalue = -1 * atoi(numstr);
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


		/* test numbers */
		if (isdigit(*expr)) {	// was expr[0]
			while (1) {
				if (!isdigit(*expr)) break;
				numstr[n] = *expr;
				expr++; n++;
			}
			if (FLAG) {
				lvalue = atoi(numstr);
				FLAG=0;
			} else
				rvalue = atoi(numstr);
			//printf("before lvalue=%d %c rvalue=%d\n",lvalue,MATHFUNC,rvalue);
			if (MATHFLAG) {
				lvalue = doArith(rvalue,lvalue,MATHFUNC);
				rvalue = 0;
				MATHFUNC = ' '; MATHFLAG=0;
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

int doArith(int rvalue,int lvalue,char MATHFUNC) {
	//printf("(doArith: lval[%d]  rval[%d]  )",lvalue,rvalue);
	if (MATHFUNC == '+') return (lvalue += rvalue);
	if (MATHFUNC == '-') return (lvalue -= rvalue);
	if (MATHFUNC == '*') return (lvalue *= rvalue);
	if (MATHFUNC == '/') return (lvalue /= rvalue);
	if (MATHFUNC == 'E') return (lvalue = pow(lvalue,rvalue));
	if (MATHFUNC == '&') return (lvalue & rvalue);
	if (MATHFUNC == '|') return (lvalue | rvalue);
	if (MATHFUNC == '^') return (lvalue ^ rvalue);

	printf("Unknown expression %c in eval routine\n",MATHFUNC);
	return 0;
}
