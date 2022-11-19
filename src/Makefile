# Do 'make eigenmath' to avoid touching prototypes.h

.PHONY: default clean

CC = gcc
CFLAGS = -Wall -O0 -include defs.h -include prototypes.h -include externs.h

default:
	make prototypes.h
	make eigenmath

prototypes.h: defs.h externs.h *.c
	make -C ../tools

objs := $(shell ls *.c | sed "s/\.c/\.o/g")

$(objs): defs.h prototypes.h externs.h

eigenmath: $(objs)
	gcc -o eigenmath $(objs) -lm

clean:
	rm -f eigenmath *.o
