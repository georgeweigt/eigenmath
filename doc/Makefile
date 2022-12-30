.PHONY: sync

# pdflatex twice for table of contents

eigenmath.pdf: *.tex
	pdflatex eigenmath.tex
	pdflatex eigenmath.tex

sync:
	cp help.html ../xcode/src/Base.lproj/Eigenmath.help/Contents/Resources/English.lproj/index.html
