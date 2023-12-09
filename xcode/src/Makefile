.PHONY: default clean

# sed removes ../../main.c and ../../fmt.c

FILES := $(shell ls ../../src/*.c | sed "s/\.\.\/\.\.\/src\/main\.c//;s/\.\.\/\.\.\/src\/fmt\.c//")

default:
	make -C ../../src prototypes.h
	make eigenmath.c
	make prototypes.h

eigenmath.c: defs.h ../../src/prototypes.h $(FILES)
	cat defs.h ../../src/prototypes.h $(FILES) > eigenmath.c

defs.h: ../../src/defs.h
	cp ../../src/defs.h .

prototypes.h: *.c
	make -s -C ../../tools make-prototypes
	../../tools/make-prototypes *.c > prototypes.h

clean:
	rm -f defs.h prototypes.h eigenmath.c
