.PHONY: check clean

eigenmath.js: src/*
	cat src/* > eigenmath.js

check:
	make -s -C ../tools wcheck
	../tools/wcheck src/*.js

clean:
	rm -f eigenmath.js
