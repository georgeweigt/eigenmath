# make prototype files

default :
	gcc make-prototypes.c
	./a.out ../src/ >../src/prototypes.h
	./a.out ../app/ >../app/app-prototypes.h
	rm a.out
