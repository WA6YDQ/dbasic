This is the readme for dbasic - an implimentation of Dartmouth BASIC.

As of Jun 2 2022 this is incomplete and I'm using github 
as my off-site backup.

To build, type 'make'

To run, use ./basic [file.bas]

Usage:

There is no built-in text editor. Use an external text editor
to create the BASIC source (and sort the lines per/line number). 

Line numbers are required, all characters (except between double
quotes "" are lower case. (expect this to change later).

Variables {a...z} are floating point numeric.
1, 21.5, -0.3, 2.4e15 are all valid

Variables {a$...z$} are string (max 80 chars).

The following BASIC keywords are valid:
REM, PRINT, LET, GOTO, ON/GOTO, GOSUB, RETURN,
END, IF/THEN, FOR/NEXT/STEP, INPUT,
DATA, READ, RESTORE, CLEAR, STOP, DEF FNx

The following functions are valid:
SIN(), COS(), TAN(), EXP(), LOG(), LOG10(), SQR() 

The following math functions are valid:
+, -, *, /, **, - (unary negative)

The following equality tests are valid:
=, !=, <>, <. <=, >, >=

The following logical tests are valid:
& (and), | (or), ^ (xor)

User functions can be defined. def fn{a..z}
An example: 
10 def fnf = "1/(6.28*sqr(l*c))"
20 let l=1.2e-6, c=120e-12
30 let f=fnf
40 print "f=";f
Results in:
f=1.32696e+07


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

NOTE: Expressions are NOT algebraic. a+b*c is NOT a+(b*c)
Evaluation is left to right. Expressions inside parens: a+(b*c) 
ARE evaluated first.
(Expect this to change to algebraic order later).

In Dartmouth, the ^ character is used for powers. IE 2^3 would
be 2*2*2. This version of BASIC uses ** instead of ^, IE 2**3

This version uses the ^ as a logical XOR (also & is logical AND
and | is logocal OR). Dartmouth didn't have logical operators.

Dartmouth used <> as not equal. This version also uses <> as well
as != for a not equal comparison.


