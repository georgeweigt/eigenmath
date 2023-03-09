.PHONY: clean check diffs

CFILES := $(shell ls src/*.c)

eigenmath: eigenmath.c
	$(CC) -Wall -O0 -o eigenmath eigenmath.c -lm

eigenmath.c: src/defs.h src/prototypes.h $(CFILES)
	cat src/defs.h src/prototypes.h $(CFILES) > eigenmath.c

clean:
	rm -f eigenmath eigenmath.c

check:
	make -s -C tools wcheck
	for FILE in src/*.c; do tools/wcheck $$FILE; done
	for FILE in js/src/*.js; do tools/wcheck $$FILE; done

diffs:
	-rm -f diffs
#
	-diff -u src/sin.c js/src/eval_sin.js >> diffs
	-diff -u src/cos.c js/src/eval_cos.js >> diffs
	-diff -u src/tan.c js/src/eval_tan.js >> diffs
#
	-diff -u src/arcsin.c js/src/eval_arcsin.js >> diffs
	-diff -u src/arccos.c js/src/eval_arccos.js >> diffs
	-diff -u src/arctan.c js/src/eval_arctan.js >> diffs
#
	-diff -u src/sinh.c js/src/eval_sinh.js >> diffs
	-diff -u src/cosh.c js/src/eval_cosh.js >> diffs
	-diff -u src/tanh.c js/src/eval_tanh.js >> diffs
#
	-diff -u src/arcsinh.c js/src/eval_arcsinh.js >> diffs
	-diff -u src/arccosh.c js/src/eval_arccosh.js >> diffs
	-diff -u src/arctanh.c js/src/eval_arctanh.js >> diffs
#
	-diff -u src/abs.c js/src/eval_abs.js > diffs
	-diff -u src/adj.c js/src/eval_adj.js >> diffs
	-diff -u src/conj.c js/src/eval_conj.js >> diffs
	-diff -u src/eigenvec.c js/src/eval_eigenvec.js >> diffs
	-diff -u src/erf.c js/src/eval_erf.js >> diffs
	-diff -u src/erfc.c js/src/eval_erfc.js >> diffs
	-diff -u src/integral.c js/src/eval_integral.js >> diffs
	-diff -u src/log.c js/src/eval_log.js >> diffs
