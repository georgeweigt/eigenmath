.PHONY: clean

eigenmath: defs.h prototypes.h externs.h *.c
	gcc -Wall -O0 -o eigenmath -include defs.h -include prototypes.h -include externs.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f eigenmath
