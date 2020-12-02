To build and run:

	cd src
	make
	./eigenmath

Press control-C or type exit to exit.

Note: Terminal settings should include UTF-8.

To run a script:

	./eigenmath scriptfilename

To generate LaTeX output:

	./eigenmath --latex scriptfilename | tee foo.tex

To generate MathML output:

	./eigenmath --mathml scriptfilename | tee foo.html

To generate MathJax output:

	./eigenmath --mathjax scriptfilename | tee foo.html

The Eigenmath manual and sample scripts are available at https://georgeweigt.github.io

A Javascript version of Eigenmath is available in the 'js' directory.
