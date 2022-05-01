.PHONY: default clean

default:
	make eigenmath.pdf
	make eigenmath-macos.pdf

# pdflatex twice for table of contents

eigenmath.pdf: *.tex
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex
	rm -f *.aux *.log *.toc

eigenmath-macos.pdf: *.tex
	pdflatex eigenmath-macos.tex
	pdflatex eigenmath-macos.tex
	rm -f *.aux *.log *.toc

clean:
	rm -f eigenmath.pdf eigenmath-macos.pdf
