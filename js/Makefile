# See demo.html for a template web page that uses eigenmath.js


objects := $(shell ls *.js | sed "s/\n/ /g;s/eigenmath\.js//")

default:
	cat preamble $(objects) > eigenmath.js

clean:
	rm -f eigenmath.js

cp:
	gsutil cp eigenmath.js gs://www.eigenmath.org
	cp eigenmath.js ~/Documents/GitHub/work
