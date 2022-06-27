This is the readme for dbasic - an implimentation of Dartmouth BASIC.
This is licensed under the MIT license. This compiles cleanly
and runs error free in valgrind.

To build, type 'make'
To install, type sudo make install
The binary 'dbasic' will be installed in /usr/local/bin

Statements in this version of BASIC:
 REM, PRINT, LET, GOTO, GOSUB, RETURN, END, 
 IF/THEN, FOR/NEXT/STEP, INPUT, READ, DATA,
 RESTORE, ON/GOTO, ON/GOSUB, CLEAR, STOP, DEF,
 OPEN, CLOSE, REWIND

Expressions: a+b, a-b, a*b, a/b, a**b, -a
 
Equality Tests: >, >=, <, <=, =, <>, !=
 
Logical:  a&b, a|b, a^b (bitwise and/or/xor)
 
Functions: 
  SIN(), COS(), TAN(), ATN(), EXP(), 
  LN(), LOG(), SQR(), LEFT$(), RIGHT$(), MID$(), 
  ASC(), CHR$(), FNx, INT(), ABS(), SGN(), LEN(),
  RND(), RND(n), RAD(), DEG(), SPC(), PI(), 
  VAL(), EOF(), STR$(), TIME()

In addition the + operator can be used to catenate
text strings (a$...z$).

Please see the file Dbasic.txt for usage instructions 
and examples.

-----------------------------------------------------------

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

There are basic program examples for each keyword. See the
file *.bas in the examples directory of this repository.

While running, hitting ctrl-C will stop execution. If the
user is in an input loop, hit ctrl-C then press the enter key.

This basic interpreter follows (mostly, exceptions below)
Dartmouth BASIC described in the book "BASIC 4th Edition"
dated January 1968. There are many additional functions
and keywords not in the original specification in this version.


--- Language exceptions ---
In the original Dartmouth BASIC you could have spaces (or
not) scattered anywhere. In dbasic keywords and functions 
must be surrounded by spaces.

In Darthmouth, GO TO is ok to use. Here, use GOTO, no spaces.
Same applied to ON..GO TO. Use ON..GOTO instead.

In Dartmouth, a statement could be written:
15LETG=A*E-B*D

Here the keyword LET (as ALL keywords) must have surrounding spaces.
15 let g=a*e-b*d
Spaces between elements are also OK:
30 let g = a  * e  - b  *  d

(By the way, LET is optional)

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


