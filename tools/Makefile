default :
	gcc make-prototypes.c
	./a.out >../src/prototypes.h
	gcc make-app-prototypes.c
	./a.out >../app/app-prototypes.h
	gcc gen.c
	./a.out >../eigenmath.c
	rm a.out
