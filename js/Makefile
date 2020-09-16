objects := $(shell ls *.js | sed "s/\n/ /g;s/eigenmath\.js//")

default:
	cat preamble $(objects) > eigenmath.js

clean:
	rm -f eigenmath.js
