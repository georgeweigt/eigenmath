.PHONY: default eigenmath.c

default:
	make make-prototypes
	./make-prototypes ../src/ >../src/prototypes.h
	./make-prototypes ../xcode/Eigenmath/C/ >../xcode/Eigenmath/C/app-prototypes.h
	make gen
	./gen >../eigenmath.c

make-prototypes: make-prototypes.c
	gcc -O0 -o make-prototypes make-prototypes.c

gen: gen.c
	gcc -O0 -o gen gen.c

eigenmath.c:
	make gen
	./gen >../eigenmath.c
