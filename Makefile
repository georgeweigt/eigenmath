.PHONY: clean

a.out: eigenmath.c
	gcc -Wall -O0 eigenmath.c -lm

eigenmath.c: src/defs.h src/prototypes.h src/*.c
	cat src/defs.h src/prototypes.h src/*.c > eigenmath.c

clean:
	rm -f eigenmath.c a.out
