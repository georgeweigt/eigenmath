.PHONY: check clean

eigenmath: defs.h prototypes.h *.c
	$(CC) -Wall -O0 -o eigenmath -include defs.h -include prototypes.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes *.c > prototypes.h

check:
	make -s -C ../tools wcheck
	../tools/wcheck *.c

clean:
	rm -f eigenmath prototypes.h
