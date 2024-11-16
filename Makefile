.PHONY: default clean all

default:
	make -C src prototypes.h
	make eigenmath

eigenmath: eigenmath.c
	$(CC) -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/LICENSE src/defs.h src/prototypes.h src/*.c
	cat src/LICENSE src/defs.h src/prototypes.h src/*.c > eigenmath.c

clean:
	rm -f eigenmath eigenmath.c

all:
	make -C src
	make -C xcode
	make -C js
	make -C test
	make eigenmath
