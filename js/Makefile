.PHONY: clean

FILES := $(shell ls src/*.js)

eigenmath.js: $(FILES)
	cat $(FILES) > eigenmath.js
	npx eslint eigenmath.js

clean:
	rm -f eigenmath.js
