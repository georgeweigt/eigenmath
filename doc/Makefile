# sed removes commas
# pdflatex twice for table of contents

eigenmath.pdf: $(shell ls -m *.tex | sed "s/,//g")
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
