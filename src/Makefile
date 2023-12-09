.PHONY: clean

eigenmath: defs.h prototypes.h *.c
	$(CC) -Wall -O0 -o eigenmath -include defs.h -include prototypes.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes *.c > prototypes.h

clean:
	rm -f eigenmath prototypes.h
