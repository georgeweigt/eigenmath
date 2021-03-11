# make prototype files

default:
	gcc make-prototypes.c
	./a.out ../src/ >../src/prototypes.h
	./a.out ../app/ >../app/app-prototypes.h
	rm a.out

eigenmath.c:
	gcc gen.c
	./a.out >eigenmath.c
	rm a.out

q4:
	gcc q.c
	./a.out >q4.txt
	rm a.out
	cp q4.txt ~/Downloads
	cp q4.txt ../../georgeweigt.github.io
	cp q4.txt ../../georgeweigt.github.io/Downloads
