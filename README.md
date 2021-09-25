To build and run:

	gcc eigenmath.c
	./a.out

Press ctrl-C to exit.

To run a script:

	./a.out scriptfilename

To generate LaTeX output:

	./a.out --latex scriptfilename | tee foo.tex

To generate MathML output:

	./a.out --mathml scriptfilename | tee foo.html

To generate MathJax output:

	./a.out --mathjax scriptfilename | tee foo.html

To build eigenmath.c:

	cd tools
	make eigenmath.c
