.PHONY: clean

eigenmath: eigenmath.c
	gcc -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/defs.h src/prototypes.h src/*.c
	cat src/defs.h src/prototypes.h src/*.c > eigenmath.c

clean:
	rm -f eigenmath.c a.out
