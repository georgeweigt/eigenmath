.PHONY: clean checkall sync

CFILES := $(shell ls *.c)

eigenmath: defs.h prototypes.h $(CFILES)
	gcc -Wall -O0 -o eigenmath -include defs.h -include prototypes.h $(CFILES) -lm

prototypes.h: $(CFILES)
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f eigenmath prototypes.h

checkall:
	make -s -C ../tools wcheck
	for FILE in *.h *.c; do ../tools/wcheck $$FILE; done

# sed removes main.c and fmt.c

XFILES := $(shell ls *.c | sed "s/main\.c//;s/fmt\.c//")

sync:
	cp defs.h ../xcode/src
	cat defs.h prototypes.h $(XFILES) > ../xcode/src/eigenmath.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes ../xcode/src > ../xcode/src/prototypes.h
