# pdflatex twice for table of contents

eigenmath.pdf: $(shell ls *.tex)
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex
	rm -f *.aux *.log *.synctex.gz *.toc

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
