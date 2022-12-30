.PHONY: default

# sed removes "../src/main.c" and "../src/fmt.c"

xsrc := $(shell ls ../src/*.c | sed "s/\.\.\/src\/main\.c//;s/\.\.\/src\/fmt\.c//")

default:
	cp ../src/defs.h ../xcode/src
	echo "#include \"app.h\"" > ../xcode/src/eigenmath.c
	cat $(xsrc) >> ../xcode/src/eigenmath.c
	make -s make-prototypes
	./make-prototypes ../xcode/src > ../xcode/src/prototypes.h

check: check.c
	gcc -O0 -o check check.c

make-prototypes: make-prototypes.c
	gcc -O0 -o make-prototypes make-prototypes.c
