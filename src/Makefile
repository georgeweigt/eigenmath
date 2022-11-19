eigenmath: defs.h externs.h *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes ./ > prototypes.h
	gcc -Wall -O0 -include defs.h -include prototypes.h -include externs.h *.c -o eigenmath -lm
