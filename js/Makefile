.PHONY: clean

src := $(shell ls *.js | sed "s/eigenmath\.js//")

eigenmath.js: $(src)
	cat ../tools/LICENSE $(src) > eigenmath.js
	make -s -C ../tools check
	../tools/check eigenmath.js
	npx eslint eigenmath.js

clean:
	rm -f eigenmath.js
