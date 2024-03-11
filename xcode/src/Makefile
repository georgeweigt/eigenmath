.PHONY: default clean

HELPBOOK = Base.lproj/Eigenmath.help/Contents/Resources/English.lproj/index.html

# sed removes ../../main.c and ../../fmt.c

FILES := $(shell ls ../../src/*.c | sed "s/\.\.\/\.\.\/src\/main\.c//;s/\.\.\/\.\.\/src\/fmt\.c//")

default:
	rsync -t ../../doc/help.html $(HELPBOOK)
	rsync -t ../../src/defs.h .
	make -C ../../src prototypes.h
	make eigenmath.c
	make prototypes.h

eigenmath.c: ../../src/LICENSE ../../src/defs.h ../../src/prototypes.h $(FILES)
	cat ../../src/LICENSE ../../src/defs.h ../../src/prototypes.h $(FILES) > eigenmath.c

prototypes.h: *.c
	make -s -C ../../tools make-prototypes
	../../tools/make-prototypes *.c > prototypes.h

clean:
	rm -f defs.h prototypes.h eigenmath.c
