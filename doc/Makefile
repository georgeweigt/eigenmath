.PHONY: default update clean

default:
	make eigenmath.pdf

# pdflatex twice for table of contents

eigenmath.pdf: *.tex
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex

update:
	cp help.html ../xcode/src/Base.lproj/Eigenmath.help/Contents/Resources/English.lproj/index.html

clean:
	rm -f $(shell ls eigenmath.* | sed "s/eigenmath\.tex//")
