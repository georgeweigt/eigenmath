.PHONY: default clean

default:
	make eigenmath.pdf

# pdflatex twice for table of contents

eigenmath.pdf: *.tex
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex
	rm -f *.aux *.log *.toc

clean:
	rm -f eigenmath.pdf
