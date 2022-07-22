.PHONY: default eigenmath.c xcode

default:
	make make-prototypes
	./make-prototypes ../src/ > ../src/prototypes.h
	./make-prototypes ../xcode/Eigenmath/C/ > ../xcode/Eigenmath/C/prototypes.h
	make eigenmath.c
	make xcode

make-prototypes: make-prototypes.c
	gcc -O0 -o make-prototypes make-prototypes.c

eigenmath.c:
	cat preamble ../src/defs.h ../src/prototypes.h ../src/*.c > ../eigenmath.c

xcode:
	cp ../src/defs.h ../src/externs.h ../xcode/Eigenmath/C
	echo "#include \"app.h\"" > ../xcode/Eigenmath/eigenmath.c
	cat \
../src/aaa-globals.c \
../src/abs.c \
../src/add.c \
../src/adj.c \
../src/alloc.c \
../src/arccos.c \
../src/arccosh.c \
../src/arcsin.c \
../src/arcsinh.c \
../src/arctan.c \
../src/arctanh.c \
../src/arg.c \
../src/bignum.c \
../src/ceiling.c \
../src/circexp.c \
../src/clock.c \
../src/cofactor.c \
../src/conj.c \
../src/contract.c \
../src/core.c \
../src/cos.c \
../src/cosh.c \
../src/defint.c \
../src/denominator.c \
../src/derivative.c \
../src/det.c \
../src/eigenvec.c \
../src/equal.c \
../src/erf.c \
../src/erfc.c \
../src/eval.c \
../src/exp.c \
../src/expcos.c \
../src/expsin.c \
../src/factor.c \
../src/factorial.c \
../src/filter.c \
../src/float.c \
../src/floor.c \
../src/for.c \
../src/gc.c \
../src/hadamard.c \
../src/imag.c \
../src/index.c \
../src/infixform.c \
../src/inner.c \
../src/integral.c \
../src/inv.c \
../src/kronecker.c \
../src/latex.c \
../src/log.c \
../src/mag.c \
../src/mathjax.c \
../src/mathml.c \
../src/minor.c \
../src/mod.c \
../src/multiply.c \
../src/noexpand.c \
../src/nroots.c \
../src/numerator.c \
../src/outer.c \
../src/polar.c \
../src/power.c \
../src/power_complex_number.c \
../src/power_minusone.c \
../src/power_natural_number.c \
../src/power_numbers.c \
../src/prefixform.c \
../src/primetab.c \
../src/print.c \
../src/product.c \
../src/quotient.c \
../src/rationalize.c \
../src/real.c \
../src/rect.c \
../src/roots.c \
../src/rotate.c \
../src/run.c \
../src/scan.c \
../src/setq.c \
../src/sgn.c \
../src/simplify.c \
../src/sin.c \
../src/sinh.c \
../src/stack.c \
../src/subst.c \
../src/sum.c \
../src/symbol.c \
../src/tan.c \
../src/tanh.c \
../src/taylor.c \
../src/tensor.c \
../src/test.c \
../src/transpose.c \
../src/userfunc.c \
>> ../xcode/Eigenmath/C/eigenmath.c
