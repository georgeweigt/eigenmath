# pdflatex twice for table of contents

default:
	make eigenmath.pdf
	make eigenmath-macos.pdf

eigenmath.pdf: $(shell ls *.tex)
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex
	make clean

eigenmath-macos.pdf: $(shell ls *.tex)
	pdflatex eigenmath-macos.tex
	pdflatex eigenmath-macos.tex
	make clean

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
