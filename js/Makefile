.PHONY: check clean

eigenmath.js: src/*
	cat src/* > eigenmath.js
	npx eslint eigenmath.js

check:
	make -s -C ../tools wcheck
	../tools/wcheck src/*.js

clean:
	rm -f eigenmath.js
