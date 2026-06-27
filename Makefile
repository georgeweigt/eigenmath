.PHONY: all clean

eigenmath: eigenmath.c
	$(CC) -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/LICENSE src/defs.h src/prototypes.h src/*.c
	cat src/LICENSE src/defs.h src/prototypes.h src/*.c > eigenmath.c

src/prototypes.h: src/*.c
	make -C src prototypes.h

all:
	make eigenmath
	make -C doc
	make -C js
	make -C test
	make -C xcode

clean:
	make -C tools clean
	rm -f eigenmath src/eigenmath
