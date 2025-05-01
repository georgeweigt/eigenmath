.PHONY: check clean

eigenmath.js: LICENSE src/*.js
	cat LICENSE src/*.js > eigenmath.js

check:
	make -s -C ../tools wcheck
	../tools/wcheck src/*.js

clean:
	rm -f eigenmath.js
