.PHONY: clean check

eigenmath: eigenmath.c
	$(CC) -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/defs.h src/prototypes.h src/*.c
	cat src/defs.h src/prototypes.h src/*.c > eigenmath.c

src/prototypes.h: src/*.c
	make -C src prototypes.h

clean:
	rm -f eigenmath eigenmath.c

check:
	make -C tools wcheck
	tools/wcheck src/*.c
	tools/wcheck js/src/*.js
