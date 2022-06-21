# To update prototypes.h, run make in the tools directory:
#
#	cd ../tools
#	make

CFLAGS = -Wall -Wextra -Werror -O0
CC = gcc

# := means evaluate then assign result to variable
# sed removes the commas then replaces .c with .o

objects := $(shell ls -m *.c | sed "s/,//g;s/\.c/\.o/g")

eigenmath : $(objects)
	gcc -o eigenmath $(objects) -lm

$(objects) : defs.h defs1.h prototypes.h

clean :
	rm -f eigenmath *.o
