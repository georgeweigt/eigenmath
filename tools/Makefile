.PHONY: default

# sed removes "../src/globals.c"

src := $(shell ls ../src/*.c | sed "s/\.\.\/src\/globals\.c//")

# sed removes "../src/globals.c", "../src/main.c", and "../src/fmt.c"

xsrc := $(shell ls ../src/*.c | sed "s/\.\.\/src\/globals\.c//;s/\.\.\/src\/main\.c//;s/\.\.\/src\/fmt\.c//")

default:
	cp ../src/defs.h ../src/externs.h ../xcode/src
	echo "#include \"app.h\"" > ../xcode/src/eigenmath.c
	cat ../src/globals.c $(xsrc) >> ../xcode/src/eigenmath.c
	make -s make-prototypes
	./make-prototypes ../xcode/src > ../xcode/src/prototypes.h
#
	cat ../src/defs.h ../src/prototypes.h ../src/globals.c $(src) > ../eigenmath.c
	make -s check
	./check ../eigenmath.c

check: check.c
	gcc -O0 -o check check.c

make-prototypes: make-prototypes.c
	gcc -O0 -o make-prototypes make-prototypes.c
