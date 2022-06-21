.PHONY: default

default:
	make make-prototypes
	make ../src/prototypes.h
	make ../xcode/Eigenmath/C/app-prototypes.h
	make gen
	make ../eigenmath.c

make-prototypes: make-prototypes.c
	gcc -O0 -o make-prototypes make-prototypes.c

../src/prototypes.h: ../src/*.c
	./make-prototypes ../src/ >../src/prototypes.h

../xcode/Eigenmath/C/app-prototypes.h: ../src/*.c
	./make-prototypes ../xcode/Eigenmath/C/ >../xcode/Eigenmath/C/app-prototypes.h

gen: gen.c
	gcc -O0 -o gen gen.c

../eigenmath.c: ../src/*.c
	./gen >../eigenmath.c
