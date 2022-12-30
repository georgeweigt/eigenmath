.PHONY: clean

eigenmath: defs.h prototypes.h *.c
	gcc -Wall -O0 -include defs.h -include prototypes.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f prototypes.h a.out
