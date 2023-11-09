.PHONY: default sync clean

FILES = introduction.tex syntax.tex symbols.tex function-definitions.tex \
        arithmetic.tex complex-numbers.tex draw.tex \
        linear-algebra.tex component-arithmetic.tex quantum-computing.tex \
        derivative.tex template-functions.tex laplacian.tex \
        integral.tex arc-length.tex line-integral.tex surface-area.tex \
        surface-integral.tex greens-theorem.tex stokes-theorem.tex \
        feature-index.tex tricks.tex

# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html

../../georgeweigt.github.io/manual/%.pdf: %.pdf
	cp $< $@

# Sometimes, must do pdflatex three times to get correct toc

%.pdf: %.tex preamble
	pdflatex $<
	if [ -e $*.toc ] ; then pdflatex $< ; pdflatex $< ; fi

# $$ sends a single $ to the shell

default:
	make eigenmath.tex
	for file in $$(ls *.tex | sed "s/\.tex/\.pdf/") ; do make $$file ; done

eigenmath.tex : a.out preamble $(FILES)
	./a.out $(FILES) > eigenmath.tex

a.out: merge.c
	$(CC) merge.c

FILE1 = ../../georgeweigt.github.io/help.html
FILE2 = ../xcode/src/Base.lproj/Eigenmath.help/Contents/Resources/English.lproj/index.html

$(FILE1): help.html
	cp help.html $(FILE1)

$(FILE2): help.html
	cp help.html $(FILE2)

sync:
	make $(FILE1)
	make $(FILE2)
	for file in *.pdf ; do make ../../georgeweigt.github.io/manual/$$file ; done

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
