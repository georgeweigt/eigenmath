.PHONY: check clean

eigenmath.js: LICENSE src/*
	cat LICENSE src/* > eigenmath.js

check:
	make -s -C ../tools wcheck
	../tools/wcheck src/*.js

clean:
	rm -f eigenmath.js
