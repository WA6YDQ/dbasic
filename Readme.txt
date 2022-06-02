This is the readme for dbasic - an implimentation of Dartmouth BASIC.

As of Jun 2 2022 this is incomplete and I'm using github 
as my off-site backup.

To build, type 'make'

To run, use ./basic [file.bas]

Usage:

There is no built-in text editor. Use an external text editor
to create the BASIC source. 

Line numbers are required, all characters (except between double
quotes "" are lower case. (expect this to change later).

Variables are {a...z} for integer numeric, {a$...z$} for
string.

The following BASIC keywords are valid:
REM
PRINT
LET
GOTO
GOSUB
RETURN
END
IF/THEN
FOR/NEXT/STEP
INPUT

