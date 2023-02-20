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
	for FILE in $$(find . -name "*.h"); do tools/wcheck $$FILE; done
	for FILE in $$(find . -name "*.c"); do tools/wcheck $$FILE; done
	for FILE in $$(find . -name "*.js"); do tools/wcheck $$FILE; done
