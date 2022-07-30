.PHONY: default update xcode

# sed removes "../src/globals.c"

objs := $(shell ls ../src/*.c | sed "s/\.\.\/src\/globals\.c//")

# sed removes "../src/globals.c", "../src/main.c", and "../src/fmt.c"

xobjs := $(shell ls ../src/*.c | sed "s/\.\.\/src\/globals\.c//;s/\.\.\/src\/main\.c//;s/\.\.\/src\/fmt\.c//")

default:
	make -s check
	make -s make-prototypes
	make -s update
	make -s xcode

check: check.c
	gcc -O0 -o check check.c

make-prototypes: make-prototypes.c
	gcc -O0 -o make-prototypes make-prototypes.c

update:
	./make-prototypes ../src/ > ../src/prototypes.h
	cat preamble ../src/defs.h ../src/prototypes.h ../src/globals.c $(objs) > ../eigenmath.c

xcode:
	cp ../src/defs.h ../src/externs.h ../xcode/src/C
	echo "#include \"app.h\"" > ../xcode/src/C/eigenmath.c
	cat ../src/globals.c $(xobjs) >> ../xcode/src/C/eigenmath.c
	./make-prototypes ../xcode/src/C/ > ../xcode/src/C/prototypes.h
