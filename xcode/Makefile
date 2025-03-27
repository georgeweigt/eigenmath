.PHONY: default clean

HELPBOOK = Base.lproj/Eigenmath.help/Contents/Resources/English.lproj/index.html

# sed removes main.c and fmt.c

FILES := $(shell ls ../src/*.c | sed "s/\.\.\/src\/main\.c//;s/\.\.\/src\/fmt\.c//")

default:
	rsync -c ../doc/help.html $(HELPBOOK)
	rsync -c ../src/defs.h .
	make eigenmath.c
	make prototypes.h
#	xcodebuild -project Eigenmath.xcodeproj
#	find build -name Eigenmath.app -print

eigenmath.c: app.h $(FILES)
	cat app.h $(FILES) > eigenmath.c

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes *.c > prototypes.h

clean:
	rm -f defs.h prototypes.h eigenmath.c
