prototypes:
	gcc make-prototypes.c
	./a.out ../src/ >../src/prototypes.h
	./a.out ../xcode/Eigenmath/C/ >../xcode/Eigenmath/C/app-prototypes.h
	rm a.out

eigenmath.c:
	gcc gen.c
	./a.out >../eigenmath.c
	rm a.out
