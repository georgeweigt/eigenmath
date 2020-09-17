objects := $(shell ls *.js | sed "s/\n/ /;s/eigenmath\.js//")

default:
	cat preamble $(objects) > eigenmath.js

clean:
	rm -f eigenmath.js

check:
	npx eslint eigenmath.js
