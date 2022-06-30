all:	basic.o print.o input.o goto.o eval.o let.o \
		ifthen.o fornext.o data.o def.o insert.o loadsave.o \
		dim.o file.o
	$(CC) $(FLAGS) -o dbasic basic.o print.o input.o goto.o \
		eval.o let.o ifthen.o fornext.o data.o def.o \
		insert.o loadsave.o dim.o file.o   $(LIBS)

clean:	
	/bin/rm -f *.o

install:
	cp dbasic /usr/local/bin/dbasic

backup:
	tar cf dbasic.tar *.c *.h Makefile *.txt examples/*

LIBS = -lm
CC = gcc
FLAGS = -Wall -pedantic

basic.o	:	basic.c
	$(CC) $(FLAGS) -c basic.c

print.o	:	print.c
	$(CC) $(FLAGS) -c print.c

input.o	:	input.c
	$(CC) $(FLAGS) -c input.c

goto.o	:	goto.c
	$(CC) $(FLAGS) -c goto.c

eval.o	:	eval.c
	$(CC) $(FLAGS) -c eval.c

let.o	:	let.c
	$(CC) $(FLAGS) -c let.c

ifthen.o	:	ifthen.c
	$(CC) $(FLAGS) -c ifthen.c

fornext.o	:	fornext.c
	$(CC) $(FLAGS) -c fornext.c

data.o	:	data.c
	$(CC) $(FLAGS) -c data.c

def.o	:	def.c
	$(CC) $(FLAGS) -c def.c

insert.o	:	insert.c
	$(CC) $(FLAGS) -c insert.c

loadsave.o	:	loadsave.c
	$(CC) $(FLAGS) -c loadsave.c

dim.o	:	dim.c
	$(CC) $(FLAGS) -c dim.c

file.o	:	file.c
	$(CC) $(FLAGS) -c file.c
