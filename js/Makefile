src := $(shell ls *.js | sed -e s/eigenmath\.js//)

eigenmath.js: LICENSE $(src)
	cat LICENSE $(src) > eigenmath.js
	npx eslint eigenmath.js
