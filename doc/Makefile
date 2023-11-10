.PHONY: default sync syncweb clean

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

HELPBOOK = ../xcode/src/Base.lproj/Eigenmath.help/Contents/Resources/English.lproj/index.html

$(HELPBOOK): help.html
	cp help.html $(HELPBOOK)

sync:
	make $(HELPBOOK)
	if [ -e ../../georgeweigt.github.io ] ; then make syncweb ; fi

QR = ../../georgeweigt.github.io/help.html

$(QR): help.html
	cp help.html $(QR)

syncweb:
	make $(QR)
	for file in *.pdf ; do make ../../georgeweigt.github.io/manual/$$file ; done

clean:
	rm -f *.aux *.log *.synctex.gz *.toc
