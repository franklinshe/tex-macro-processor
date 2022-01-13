CC = gcc
CFLAGS = -Wall -g3 -std=c11 -pedantic
FILE = test.txt

all: proj1

statemachine.o: statemachine.c statemachine.h

macros.o: macros.c macros.h proj1.h

proj1.o: proj1.c proj1.h statemachine.h macros.h

proj1: proj1.o statemachine.o macros.o

clean:
	rm -rf proj1 *.o

test:
	/usr/bin/valgrind -q ./proj1 < $(FILE) > test.me && ../proj1 < $(FILE) > test.out && diff test.me test.out