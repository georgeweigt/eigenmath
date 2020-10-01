
Eigenmath is primarily a C program.

A Javascript version for web pages is available in the 'js' directory.

To build and run the C version:

	cd src
	make
	./eigenmath

Press control-C or type exit to exit.

To run a script:

	./eigenmath scriptfilename

Sample scripts are available at eigenmath.github.io

To generate LaTeX output:

	./eigenmath --latex scriptfilename | tee foo.tex

To generate MathML output:

	./eigenmath --mathml scriptfilename | tee foo.html

To generate MathJax output:

	./eigenmath --mathjax scriptfilename | tee foo.html

MathML and MathJax results look best on Firefox.

History:

July 29, 2020
Use cross multiplication to test for equality.

July 30, 2020
Remove eval quirk in user defined functions.

September 12, 2020
Port to Javascript.
