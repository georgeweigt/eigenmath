.PHONY: clean

CFILES := $(shell ls src/*.c)

eigenmath: eigenmath.c
	gcc -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/defs.h src/prototypes.h $(CFILES)
	cat src/defs.h src/prototypes.h $(CFILES) > eigenmath.c

clean:
	rm -f eigenmath eigenmath.c
