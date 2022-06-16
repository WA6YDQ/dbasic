This is the readme for dbasic - an implimentation of Dartmouth BASIC.

As of Jun 16 2022 this is fully functional but
incomplete. There are still some functions and 
keywords I'll be adding.
(I'm using github as my off-site backup).

To build, type 'make'
To install, type sudo make install
The binary 'dbasic' will be installed in /usr/local/bin

To start, type dbasic [file.bas] or dbasic (no file given)

Usage:

If started with a basic file as the second arguement, the
basic file will be loaded and executed. dbasic will terminate
when the basic program ends.

If no filename is given, a Ready prompt will be displayed
and interactive mode is started. You can then enter basic 
lines or commands.

These are the basic commands dbasic understands:
save [filename]	(save the buffer to a disk file)
load [filename]	(load a disk file to the buffer)
new 		(delete the buffer)
free		(show available buffer memory)
list		(display the basic lines in the buffer)
run		(start running the basic program)
exit & quit	(exit dbasic w/o saving)


Line numbers are required, all characters (except between double
quotes "" are lower case. (this may change later).

Entering a line will save the line in the buffer.
Entering a line number by itself will delete an existing line. 
Entering a line with an existing line number will replace the 
original line.

Statements in this version of BASIC:
 REM, PRINT, LET, GOTO, GOSUB, RETURN, END, 
 IF/THEN, FOR/NEXT/STEP, INPUT, READ, DATA,
 RESTORE, ON/GOTO, CLEAR, STOP, DEF FNx

 Formulas: a+b, a-b, a*b, a/b, a**b, -a
 
 Equality Tests: >, >=, <, <=, =, <>, !=
 
 Logical:  a&b, a|b, a^b (bitwise and/or/xor)
 
 Functions: SIN(), COS(), TAN(), EXP(), LN(),
 LOG(), SQR(), LEFT$(), RIGHT$(), MID$(), ASC(),
 CHR$(), FNx, INT(), ABS(), SGN(), ATN(), LEN()

Floating point numeric variables are a...z
String variables are a$...z$

Numeric variables and string variables do not share the
same memory space.

Numeric variables are pre-defined as arrays without a DIM
statement. a is the same as a(0), and a(0) thru a(10) are 
initialized to 0 at program start (as are b() thru z(). 
The dim statement can be used to make larger arrays.
 
User functions can be defined. def fn{a..z}
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

There are basic program examples for each keyword. See the
file *.bas in the examples directory of this repository.

This basic interpreter follows (mostly, exceptions below)
Dartmouth BASIC described in the book "BASIC 4th Edition"
dated January 1968.





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
(as are b(), c(), thru z(). 

In Dartmouth, the ^ character is used for powers. IE 2^3 would
be 2*2*2. This version of BASIC uses ** instead of ^, IE 2**3

This version uses the ^ as a logical XOR (also & is logical AND
and | is logocal OR). Dartmouth didn't have logical operators.

Dartmouth used <> as not equal. This version also uses <> as well
as != for a not equal comparison.

--- Usage Notes ---

You may get slightly differing results when calculating defined functions
vs calculating directly. I believe this is due to rounding errors
in the floating point routines of the clib libraries.
More investigation is needed.
 
