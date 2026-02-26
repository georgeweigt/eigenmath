.PHONY: all

eigenmath: eigenmath.c
	$(CC) -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/LICENSE src/defs.h src/prototypes.h src/*.c
	cat src/LICENSE src/defs.h src/prototypes.h src/*.c > eigenmath.c
all:
	make -C doc
	make -C src
	make -C js
	make -C test
	make -C xcode
	make eigenmath.c
	make eigenmath
