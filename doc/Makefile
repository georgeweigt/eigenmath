.PHONY: default clean

FILES = introduction.tex syntax.tex symbols.tex function-definitions.tex \
        arithmetic.tex complex-numbers.tex draw.tex tricks.tex \
        linear-algebra.tex component-arithmetic.tex quantum-computing.tex \
        derivative.tex template-functions.tex laplacian.tex \
        integral.tex arc-length.tex line-integral.tex surface-area.tex \
        surface-integral.tex greens-theorem.tex stokes-theorem.tex \
        feature-index.tex

# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html

# Sometimes, must do pdflatex three times to get correct toc

%.pdf: %.tex preamble
	pdflatex $<
	if [ -e $*.toc ] ; then pdflatex $< ; pdflatex $< ; fi

# $$ sends a single $ to the shell

default:
	make eigenmath.tex # in case eigenmath.tex does not exist
	for file in $$(ls *.tex | sed "s/\.tex/\.pdf/") ; do make $$file ; done

eigenmath.tex : a.out preamble $(FILES)
	./a.out $(FILES) > eigenmath.tex

a.out: merge.c
	$(CC) merge.c

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
