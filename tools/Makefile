.PHONY: default clean

default:
	make check
	make make-prototypes

check: check.c
	gcc -Wall -O0 -o check check.c

make-prototypes: make-prototypes.c
	gcc -Wall -O0 -o make-prototypes make-prototypes.c

clean:
	rm -f check make-prototypes
