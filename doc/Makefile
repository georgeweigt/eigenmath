.PHONY: clean

eigenmath.pdf: eigenmath.tex
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex

FILES =	introduction.tex syntax.tex \
	symbols.tex units-of-measure.tex function-definitions.tex \
	arithmetic.tex complex-numbers.tex draw.tex \
	linear-algebra.tex linear-regression.tex \
	component-arithmetic.tex quantum-computing.tex \
	derivative.tex template-functions.tex laplacian.tex \
	integral.tex arc-length.tex line-integral.tex surface-area.tex \
	surface-integral.tex tricks.tex index.tex

eigenmath.tex: $(FILES)
	(cat preamble ; for FILE in $(FILES) ; do sed "1d;\$$d" $$FILE ; done ; echo "\\end{document}") > eigenmath.tex

clean:
	rm -f *.aux *.log *.out *.synctex.gz *.toc
