.PHONY: default xcode

default:
	make -s xcode
	make -s check
	make -s make-prototypes
	./make-prototypes ../src/ > ../src/prototypes.h
	./make-prototypes ../xcode/src/C/ > ../xcode/src/C/prototypes.h
	cat preamble ../src/defs.h ../src/prototypes.h ../src/*.c > ../eigenmath.c

check: check.c
	gcc -O0 -o check check.c

make-prototypes: make-prototypes.c
	gcc -O0 -o make-prototypes make-prototypes.c

xcode:
	rm -rf ../xcode/Eigenmath.xcodeproj/project.xcworkspace
	rm -rf ../xcode/Eigenmath.xcodeproj/xcuserdata
	cp ../src/defs.h ../src/externs.h ../xcode/src/C
	echo "#include \"app.h\"" > ../xcode/src/C/eigenmath.c
	echo "#define XCODE 1" >> ../xcode/src/C/eigenmath.c
	cat ../src/*.c >> ../xcode/src/C/eigenmath.c
