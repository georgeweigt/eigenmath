.PHONY: default

# sed removes "../src/main.c" and "../src/fmt.c"

files := $(shell ls ../src/*.c | sed "s/\.\.\/src\/main\.c//;s/\.\.\/src\/fmt\.c//")

default:
	cp ../src/defs.h src
	cat preamble $(files) > src/eigenmath.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes src > src/prototypes.h
