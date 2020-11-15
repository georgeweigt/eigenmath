# sed removes commas
# pdflatex twice for table of contents

eigenmath.pdf: $(shell ls -m *.tex | sed "s/,//g")
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex
	rm -f *.aux *.log *.synctex.gz *.toc

clean:
	rm -f eigenmath.pdf *.aux *.log *.synctex.gz *.toc
