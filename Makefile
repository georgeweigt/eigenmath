.PHONY: clean check

CFILES := $(shell ls src/*.c)

eigenmath: eigenmath.c
	gcc -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/defs.h src/prototypes.h $(CFILES)
	cat src/defs.h src/prototypes.h $(CFILES) > eigenmath.c

clean:
	rm -f eigenmath eigenmath.c

check:
	make -s -C tools wcheck
	for FILE in src/*.c; do tools/wcheck $$FILE; done
	for FILE in js/src/*.js; do tools/wcheck $$FILE; done
