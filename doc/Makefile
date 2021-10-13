# pdflatex twice for table of contents

default: $(shell ls *.tex)
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex
	make clean
	pdflatex eigenmath-macos.tex
	pdflatex eigenmath-macos.tex
	make clean

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
