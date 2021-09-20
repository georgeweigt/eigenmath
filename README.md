The Eigenmath manual and sample scripts are available at https://georgeweigt.github.io

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

Eigenmath uses UTF-8 encoding to display Greek letters and large delimiters.

	      ┌                                  ┐
	      │ −ξ(r)     0      0        0      │
	      │                                  │
	      │           1                      │
	      │   0     ╶────╴   0        0      │
	      │          ξ(r)                    │
	g   = │                                  │
	 μν   │                   2              │
	      │   0       0      r        0      │
	      │                                  │
	      │                        2       2 │
	      │   0       0      0    r  sin(θ)  │
	      └                                  ┘

To build eigenmath.c:

	cd tools
	make eigenmath.c
