.PHONY: clean
CC = gcc
CFLAGS = -O0 -include defs.h -include prototypes.h -include externs.h

objs := $(shell ls *.c | sed "s/\.c/\.o/g")

eigenmath: $(objs) defs.h prototypes.h externs.h
	gcc -o eigenmath $(objs) -lm

clean:
	rm -f eigenmath *.o
