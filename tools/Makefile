# build prototype files
default:
	gcc make-prototypes.c
	./a.out ../src/ >../src/prototypes.h
	./a.out ../app/Eigenmath/C/ >../app/Eigenmath/C/app-prototypes.h
	rm a.out

eigenmath.c:
	gcc gen.c
	./a.out >../eigenmath.c
	rm a.out
