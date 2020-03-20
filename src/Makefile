CFLAGS = -Wall -O0
CC = gcc

objects = $(shell ls -m *.c | sed "s/,//g;s/\.c/\.o/g")

eigenmath : $(objects)
	gcc -o eigenmath $(objects) -lm

$(objects) : defs.h defs1.h prototypes.h

clean :
	rm -f eigenmath *.o
