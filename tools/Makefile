.PHONY: default clean

default:
	make wcheck
	make make-prototypes

wcheck: wcheck.c
	$(CC) -Wall -O0 -o wcheck wcheck.c

make-prototypes: make-prototypes.c
	$(CC) -Wall -O0 -o make-prototypes make-prototypes.c

clean:
	rm -f wcheck make-prototypes
