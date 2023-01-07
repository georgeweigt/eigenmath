.PHONY: clean sync

eigenmath: defs.h prototypes.h *.c
	gcc -Wall -O0 -o eigenmath -include defs.h -include prototypes.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f prototypes.h a.out

sync:

# sed removes "../src/main.c" and "../src/fmt.c"

files := $(shell ls ../src/*.c | sed "s/\.\.\/src\/main\.c//;s/\.\.\/src\/fmt\.c//")

sync:
	cp defs.h ../xcode/src
	cat defs.h prototypes.h $(files) > ../xcode/src/eigenmath.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes ../xcode/src > ../xcode/src/prototypes.h
