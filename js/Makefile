.PHONY: clean

eigenmath.js: src/*
	cat src/* > eigenmath.js
	npx eslint eigenmath.js

clean:
	rm -f eigenmath.js
