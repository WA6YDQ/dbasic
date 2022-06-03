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

Variables are {a...z} are integer numeric, {a$...z$} are
string (max 80 chars).

The following BASIC keywords are valid:
REM
PRINT
LET
GOTO
ON/GOTO
GOSUB
RETURN
END
IF/THEN
FOR/NEXT/STEP
INPUT
DATA
READ
RESTORE

There are basic program examples for each keyword. See the
file *.bas in this repository.

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
15 LET G=A*E-B*D

In Dartmouth, the ^ character is used for powers. IE 2^3 would
be 2*2*2. This version of BASIC uses ** instead of ^, IE 2**3

This version uses the ^ as a logical XOR (also & is logical AND
and | is logocal OR). Dartmouth didn't have logical operators.

Dartmouth used <> as not equal. This version also uses <> as well
as != for a not equal comparison.


