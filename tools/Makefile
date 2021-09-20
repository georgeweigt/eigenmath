.PHONY: default eigenmath.c

default:
	gcc make-prototypes.c
	./a.out ../src/ >../src/prototypes.h
	./a.out ../app/ >../app/app-prototypes.h
	rm a.out

eigenmath.c:
	gcc gen.c
	./a.out >../eigenmath.c
	rm a.out
