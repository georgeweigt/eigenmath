.PHONY: default clean

default:
	make prototypes.h
	make eigenmath

prototypes.h: defs.h externs.h *.c
	make -C ../tools

CC = gcc
CFLAGS = -Wall -O0 -include defs.h -include prototypes.h -include externs.h

objs := $(shell ls *.c | sed "s/\.c/\.o/g")

eigenmath: $(objs)
	gcc -o eigenmath $(objs) -lm

$(objs): defs.h prototypes.h externs.h

clean:
	rm -f eigenmath *.o
