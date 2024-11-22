.PHONY: default clean

%.pdf: %.tex preamble
	pdflatex $<
	if [ -e $*.toc ] ; then pdflatex $< ; pdflatex $< ; fi

# make eigenmath.tex in case eigenmath.tex doesn't exist

default:
	make eigenmath.tex
	for file in $$(basename -a -s .tex *.tex) ; do make $$file.pdf ; done

FILES =	title.tex \
	introduction.tex syntax.tex symbols.tex function-definitions.tex \
        arithmetic.tex complex-numbers.tex draw.tex \
        linear-algebra.tex component-arithmetic.tex quantum-computing.tex \
        derivative.tex template-functions.tex laplacian.tex \
        integral.tex arc-length.tex line-integral.tex surface-area.tex \
        surface-integral.tex greens-theorem.tex stokes-theorem.tex \
        feature-index.tex tricks.tex

eigenmath.tex: unify preamble $(FILES)
	./unify $(FILES) > eigenmath.tex

unify: unify.c
	$(CC) -o unify unify.c

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
