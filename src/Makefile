# To update prototypes.h, run make in the tools directory:
#
#	cd ../tools
#	make

CFLAGS = -Wall -Wextra -Werror -O0 -include defs.h -include prototypes.h -include externs.h
CC = gcc

# := means evaluate then assign result to variable
# sed removes the commas then replaces .c with .o

objects := $(shell ls *.c | sed "s/\.c/\.o/g")

eigenmath : $(objects)
	gcc -o eigenmath $(objects) -lm

$(objects) : defs.h prototypes.h externs.h

clean :
	rm -f eigenmath *.o
