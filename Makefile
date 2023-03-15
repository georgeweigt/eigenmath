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
	-diff -u src/tan.c js/src/eval_tan.js >> diffs
#
	-diff -u src/sinh.c js/src/eval_sinh.js >> diffs
	-diff -u src/tanh.c js/src/eval_tanh.js >> diffs
#
	-diff -u src/abs.c js/src/eval_abs.js >> diffs
	-diff -u src/adj.c js/src/eval_adj.js >> diffs
	-diff -u src/and.c js/src/eval_and.js >> diffs
	-diff -u src/arccos.c js/src/eval_arccos.js >> diffs
	-diff -u src/arccosh.c js/src/eval_arccosh.js >> diffs
	-diff -u src/arcsin.c js/src/eval_arcsin.js >> diffs
	-diff -u src/arcsinh.c js/src/eval_arcsinh.js >> diffs
	-diff -u src/arctan.c js/src/eval_arctan.js >> diffs
	-diff -u src/arctanh.c js/src/eval_arctanh.js >> diffs
	-diff -u src/arg.c js/src/eval_arg.js >> diffs
	-diff -u src/binding.c js/src/eval_binding.js >> diffs
	-diff -u src/ceiling.c js/src/eval_ceiling.js >> diffs
	-diff -u src/check.c js/src/eval_check.js >> diffs
	-diff -u src/circexp.c js/src/eval_circexp.js >> diffs
	-diff -u src/clear.c js/src/eval_clear.js >> diffs
	-diff -u src/clock.c js/src/eval_clock.js >> diffs
	-diff -u src/cofactor.c js/src/eval_cofactor.js >> diffs
	-diff -u src/conj.c js/src/eval_conj.js >> diffs
	-diff -u src/contract.c js/src/eval_contract.js >> diffs
	-diff -u src/cos.c js/src/eval_cos.js >> diffs
	-diff -u src/cosh.c js/src/eval_cosh.js >> diffs
	-diff -u src/defint.c js/src/eval_defint.js >> diffs
	-diff -u src/derivative.c js/src/eval_derivative.js >> diffs
	-diff -u src/denominator.c js/src/eval_denominator.js >> diffs
	-diff -u src/det.c js/src/eval_det.js >> diffs
	-diff -u src/dim.c js/src/eval_dim.js >> diffs
	-diff -u src/do.c js/src/eval_do.js >> diffs
	-diff -u src/eigenvec.c js/src/eval_eigenvec.js >> diffs
	-diff -u src/erf.c js/src/eval_erf.js >> diffs
	-diff -u src/erfc.c js/src/eval_erfc.js >> diffs
	-diff -u src/eval_eval.c js/src/eval_eval.js >> diffs
	-diff -u src/exp.c js/src/eval_exp.js >> diffs
	-diff -u src/expcos.c js/src/eval_expcos.js >> diffs
	-diff -u src/expcosh.c js/src/eval_expcosh.js >> diffs
	-diff -u src/expsin.c js/src/eval_expsin.js >> diffs
	-diff -u src/expsinh.c js/src/eval_expsinh.js >> diffs
	-diff -u src/exptan.c js/src/eval_exptan.js >> diffs
	-diff -u src/exptanh.c js/src/eval_exptanh.js >> diffs
	-diff -u src/factorial.c js/src/eval_factorial.js >> diffs
	-diff -u src/float.c js/src/eval_float.js >> diffs
	-diff -u src/floor.c js/src/eval_floor.js >> diffs
	-diff -u src/for.c js/src/eval_for.js >> diffs
	-diff -u src/hadamard.c js/src/eval_hadamard.js >> diffs
	-diff -u src/imag.c js/src/eval_imag.js >> diffs
	-diff -u src/inner.c js/src/eval_inner.js >> diffs
	-diff -u src/integral.c js/src/eval_integral.js >> diffs
	-diff -u src/inv.c js/src/eval_inv.js >> diffs
	-diff -u src/kronecker.c js/src/eval_kronecker.js >> diffs
	-diff -u src/log.c js/src/eval_log.js >> diffs
	-diff -u src/mag.c js/src/eval_mag.js >> diffs
	-diff -u src/minor.c js/src/eval_minor.js >> diffs
	-diff -u src/minormatrix.c js/src/eval_minormatrix.js >> diffs
	-diff -u src/mod.c js/src/eval_mod.js >> diffs
	-diff -u src/noexpand.c js/src/eval_noexpand.js >> diffs
	-diff -u src/not.c js/src/eval_not.js >> diffs
	-diff -u src/nroots.c js/src/eval_nroots.js >> diffs
	-diff -u src/numerator.c js/src/eval_numerator.js >> diffs
	-diff -u src/or.c js/src/eval_or.js >> diffs
	-diff -u src/outer.c js/src/eval_outer.js >> diffs
	-diff -u src/polar.c js/src/eval_polar.js >> diffs
	-diff -u src/power.c js/src/eval_power.js >> diffs
	-diff -u src/print.c js/src/eval_print.js >> diffs
	-diff -u src/product.c js/src/eval_product.js >> diffs
	-diff -u src/quote.c js/src/eval_quote.js >> diffs
	-diff -u src/rank.c js/src/eval_rank.js >> diffs
	-diff -u src/rationalize.c js/src/eval_rationalize.js >> diffs
	-diff -u src/real.c js/src/eval_real.js >> diffs
	-diff -u src/rect.c js/src/eval_rect.js >> diffs
	-diff -u src/roots.c js/src/eval_roots.js >> diffs
#
#	-diff -u src/divisor.c js/src/divisor.js >> diffs
