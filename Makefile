all	:	basic.o print.o input.o goto.o gosub.o eval.o let.o \
		ifthen.o fornext.o
	$(CC) -o basic basic.o print.o input.o goto.o gosub.o \
		eval.o let.o ifthen.o fornext.o $(LIBS)

LIBS = -lm
CC = gcc

basic.o	:	basic.c
	cc -c basic.c

interp.o	:	interp.c
	cc -c interp.c

print.o	:	print.c
	cc -c print.c

input.o	:	input.c
	cc -c input.c

goto.o	:	goto.c
	cc -c goto.c

gosub.o	:	gosub.c
	cc -c gosub.c

eval.o	:	eval.c
	cc -c eval.c 

let.o	:	let.c
	cc -c let.c

ifthen.o	:	ifthen.c
	cc -c ifthen.c

fornext.o	:	fornext.c
	cc -c fornext.c


