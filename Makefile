eigenmath: eigenmath.c
	gcc -O0 -o eigenmath eigenmath.c -lm

clean:
	rm eigenmath
