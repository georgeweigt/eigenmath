.PHONY: clean

src := $(shell ls *.js | sed "s/eigenmath\.js//")

eigenmath.js: $(src)
	cat $(src) > eigenmath.js
	make -s -C ../tools wcheck
	../tools/wcheck eigenmath.js
	npx eslint eigenmath.js

clean:
	rm -f eigenmath.js
