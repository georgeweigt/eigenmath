.PHONY: clean

eigenmath: eigenmath.c
	gcc -O0 -o eigenmath eigenmath.c -lc -lm

clean:
	rm -f eigenmath
