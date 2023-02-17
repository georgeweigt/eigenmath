.PHONY: clean checkall

FILES := $(shell ls src/*.js)

eigenmath.js: $(FILES)
	cat $(FILES) > eigenmath.js
	npx eslint eigenmath.js

clean:
	rm -f eigenmath.js

checkall:
	make -s -C ../tools wcheck
	for FILE in $(FILES); do ../tools/wcheck $$FILE; done
