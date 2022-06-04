all	:	basic.o print.o input.o goto.o gosub.o eval.o let.o \
		ifthen.o fornext.o data.o
	$(CC) $(FLAGS) -o basic basic.o print.o input.o goto.o gosub.o \
		eval.o let.o ifthen.o fornext.o data.o $(LIBS)

LIBS = -lm
CC = gcc
FLAGS = -Wall

basic.o	:	basic.c
	$(CC) -c basic.c

print.o	:	print.c
	$(CC) -c print.c

input.o	:	input.c
	$(CC) -c input.c

goto.o	:	goto.c
	$(CC) -c goto.c

gosub.o	:	gosub.c
	$(CC) -c gosub.c

eval.o	:	eval.c
	$(CC) -c eval.c 

let.o	:	let.c
	$(CC) -c let.c

ifthen.o	:	ifthen.c
	$(CC) -c ifthen.c

fornext.o	:	fornext.c
	$(CC) -c fornext.c

data.o	:	data.c
	$(CC) -c data.c

