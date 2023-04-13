.PHONY: clean sync

eigenmath: defs.h prototypes.h *.c
	$(CC) -Wall -O0 -o eigenmath -include defs.h -include prototypes.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f eigenmath prototypes.h

# sed removes main.c and fmt.c

FILES := $(shell ls *.c | sed "s/main\.c//;s/fmt\.c//")

sync:
	make -s prototypes.h
	cp defs.h ../xcode/src
	cat defs.h prototypes.h $(FILES) > ../xcode/src/eigenmath.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes ../xcode/src > ../xcode/src/prototypes.h
