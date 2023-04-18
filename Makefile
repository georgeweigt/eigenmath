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
	make -s -C tools wcheck
	for FILE in src/*.c; do tools/wcheck $$FILE; done
	for FILE in js/src/*.js; do tools/wcheck $$FILE; done
