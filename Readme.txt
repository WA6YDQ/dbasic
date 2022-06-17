This is the readme for dbasic - an implimentation of Dartmouth BASIC.
This is licensed under the MIT license. This compiles cleanly
and makes valgrind happy.

To build, type 'make'
To install, type sudo make install
The binary 'dbasic' will be installed in /usr/local/bin

Statements in this version of BASIC:
 REM, PRINT, LET, GOTO, GOSUB, RETURN, END, 
 IF/THEN, FOR/NEXT/STEP, INPUT, READ, DATA,
 RESTORE, ON/GOTO, CLEAR, STOP, DEF FNx

Expressions: a+b, a-b, a*b, a/b, a**b, -a
 
Equality Tests: >, >=, <, <=, =, <>, !=
 
Logical:  a&b, a|b, a^b (bitwise and/or/xor)
 
Functions: 
  SIN(), COS(), TAN(), ATN(), EXP(), 
  LN(), LOG(), SQR(), LEFT$(), RIGHT$(), MID$(), 
  ASC(), CHR$(), FNx, INT(), ABS(), SGN(), LEN()


To start, type dbasic [file.bas] or dbasic (no file given)

If started with a BASIC file as the second arguement, the
BASIC file will be loaded and executed. dbasic will terminate
when the basic program ends.

If no filename is given, a Ready prompt will be displayed
and interactive mode is started. You can then enter BASIC 
lines or commands.

These are the interactive user commands dbasic understands:

save [filename]	(save the BASIC program to disk)
load [filename]	(load a BASIC program from disk)
new 		(delete the buffer)
free		(show available buffer memory)
list		(display the basic lines in the buffer)
run		(start running the basic program)
exit		(exit dbasic w/o saving)
quit		(exit dbasic w/o saving)


Line numbers are required. All lines entered or loaded
from a file are converted to lower case (except between
double quotes "" are left alone). All user commands 
are lower case. Filenames for save and load can be upper
or lower case.

Entering a line will save the line in the buffer.
Entering a line number by itself will delete an existing line. 
Entering a line with an existing line number will replace the 
original line.


Numeric variables are single letter, a...z
All numeric variables are floating point. Use
the INT() function to change the type.

String variables are single letter, a$...z$

Numeric variables and string variables do not share the
same memory space.

Numeric variables a(0) thru a(10) are pre-defined as 
arrays without a DIM statement (as are b(0)...b(10), c(0)...c(10),
etc thru z(0)...z(10).
To make the arrays larger use the DIM statement. Array size
is limited only by available memory: a(1e6) is a valid statement.
Also, arrays can be dimmed more than once in a program. Existing
array values remain unchanged.

 
Twenty six user functions can be defined: def fn{a..z}
An example: 
10 def fnf 1/(6.28*sqr(l*c))
20 let l=1.2e-6, c=120e-12
30 let f=fnf
40 print "f=";f
Results in:
f=1.32696e+07

All expressions are algabraic in order:
Unary negation (-1) is highest.
Expressions in parens are next: (a+b)*c 
(a+b)*c is different from a+b*c. Parens can be nested.
Powers have the next highest priority: 2**8
Next is multiplication and division: 3*6/4
Then is addition and subtraction: 4+5-3+9
Lowest priority are logical functions (&, |, ^) 

Parenthesises can be nested:
10 let a=(1+2)*(2+(3*4+(43821+5)))
20 print a
99 end
run
131520


There are basic program examples for each keyword. See the
file *.bas in the examples directory of this repository.

This basic interpreter follows (mostly, exceptions below)
Dartmouth BASIC described in the book "BASIC 4th Edition"
dated January 1968. There are many additional functions
and keywords not in the original specification in this version.


--- Language exceptions ---
In the original Dartmouth BASIC you could have spaces (or
not) scattered anywhere. This is ugly. Don't write ugly code.

In Darthmouth, GO TO is ok to use. Here, use GOTO, no spaces.
Same applied to ON..GO TO. Use ON..GOTO instead.

In Dartmouth, a statement could be written:
15LETG=A*E-B*D

Here the keyword LET (as ALL keywords) must have surrounding spaces.
15 let g=a*e-b*d
Spaces between elements are also OK:
30 let g = a  * e  - b  *  d

Dartmouth BASIC has a...z and a0, a1, c3, d2, etc as numeric
variables. Here we only have a...z with no second numeric
character. Arrays are the same with a(0) thru a(10) pre-defined
(as are b(), c(), thru z(). Use dim a(n), b(n) if you need larger 
arrays. There are no maximum array sizes.

In Dartmouth, the ^ character is used for powers. IE 2^3 would
be 2*2*2. This version of BASIC uses ** instead of ^, IE 2**3

This version uses the ^ as a logical XOR (also & is logical AND
and | is logocal OR). Dartmouth didn't have logical operators.

Dartmouth used <> as not equal. This version also uses <> as well
as != for a not equal comparison.

--- Usage Notes ---

You may get slightly differing results when calculating using 
defined functions vs calculating directly. This is due to rounding 
errors in the floating point routines of the clib libraries.



