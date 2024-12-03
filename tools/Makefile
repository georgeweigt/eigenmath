.PHONY: default clean

OBJS := $(shell basename -a -s .c *.c)

# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html

%: %.c
	$(CC) -Wall -O0 -o $@ $<

default:
	for FILE in $(OBJS) ; do make $$FILE ; done

clean:
	rm -f $(OBJS)
