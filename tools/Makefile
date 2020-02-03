default :
	gcc make-prototypes.c
	./a.out >../src/prototypes.h
	gcc gen.c
	./a.out >../eigenmath.c
	rm a.out
