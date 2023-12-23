.PHONY: check clean

eigenmath.js: src/LICENSE src/*.js
	cat src/LICENSE src/*.js > eigenmath.js
	npx eslint eigenmath.js

check:
	make -s -C ../tools wcheck
	../tools/wcheck src/*.js

clean:
	rm -f eigenmath.js
