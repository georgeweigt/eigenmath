.PHONY: default clean check

default:
	make -C src prototypes.h
	make eigenmath

eigenmath: eigenmath.c
	$(CC) -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/defs.h src/prototypes.h src/*.c
	cat src/defs.h src/prototypes.h src/*.c > eigenmath.c

clean:
	rm -f eigenmath eigenmath.c

check:
	make -s -C tools wcheck
	tools/wcheck src/*.c
	tools/wcheck js/src/*.js
