all:	basic.o print.o input.o goto.o math.o let.o \
		ifthen.o fornext.o data.o def.o insert.o loadsave.o \
		dim.o
	$(CC) $(FLAGS) -o dbasic basic.o print.o input.o goto.o \
		math.o let.o ifthen.o fornext.o data.o def.o \
		insert.o loadsave.o dim.o  $(LIBS)

clean:	
	/bin/rm -f *.o

install:
	cp dbasic /usr/local/bin/dbasic

LIBS = -lm
CC = gcc
FLAGS = -Wall -g

basic.o	:	basic.c
	$(CC) $(FLAGS) -c basic.c

print.o	:	print.c
	$(CC) $(FLAGS) -c print.c

input.o	:	input.c
	$(CC) $(FLAGS) -c input.c

goto.o	:	goto.c
	$(CC) $(FLAGS) -c goto.c

#gosub.o	:	gosub.c
#	$(CC) $(FLAGS) -c gosub.c

# the functions in eval.c are depreciated
# in favor of the functioons in math.c
#eval.o	:	eval.c
#	$(CC) $(FLAGS) -c eval.c 

math.o	:	math.c
	$(CC) $(FLAGS) -c math.c

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
