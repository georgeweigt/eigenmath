// generate eigenmath.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int prev = '\n';

void emit_file(char *);
void emit_line(char *);
void check_line(char *, int, char *);

int
main()
{
	system("cat preamble.h");

	emit_file("../src/defs1.h");
	emit_file("../src/prototypes.h");

	emit_file("../src/globals.c");
	emit_file("../src/main.c");
	emit_file("../src/run.c");
	emit_file("../src/scan.c");
	emit_file("../src/eval.c");
	emit_file("../src/display.c");

	emit_file("../src/abs.c");
	emit_file("../src/add.c");
	emit_file("../src/adj.c");
	emit_file("../src/arccos.c");
	emit_file("../src/arccosh.c");
	emit_file("../src/arcsin.c");
	emit_file("../src/arcsinh.c");
	emit_file("../src/arctan.c");
	emit_file("../src/arctanh.c");
	emit_file("../src/arg.c");
	emit_file("../src/atomize.c");
	emit_file("../src/bake.c");
	emit_file("../src/besselj.c");
	emit_file("../src/bessely.c");
	emit_file("../src/binomial.c");
	emit_file("../src/ceiling.c");
	emit_file("../src/choose.c");
	emit_file("../src/circexp.c");
	emit_file("../src/clock.c");
	emit_file("../src/coeff.c");
	emit_file("../src/cofactor.c");
	emit_file("../src/conj.c");
	emit_file("../src/contract.c");
	emit_file("../src/cos.c");
	emit_file("../src/cosh.c");
	emit_file("../src/defint.c");
	emit_file("../src/degree.c");
	emit_file("../src/denominator.c");
	emit_file("../src/derivative.c");
	emit_file("../src/det.c");
	emit_file("../src/distill.c");
	emit_file("../src/divisors.c");
	emit_file("../src/eigen.c");
	emit_file("../src/erf.c");
	emit_file("../src/erfc.c");
	emit_file("../src/exp.c");
	emit_file("../src/expand.c");
	emit_file("../src/expcos.c");
	emit_file("../src/expsin.c");
	emit_file("../src/factor.c");
	emit_file("../src/factorial.c");
	emit_file("../src/factorpoly.c");
	emit_file("../src/factors.c");
	emit_file("../src/filter.c");
	emit_file("../src/float.c");
	emit_file("../src/floor.c");
	emit_file("../src/for.c");
	emit_file("../src/gcd.c");
	emit_file("../src/guess.c");
	emit_file("../src/hermite.c");
	emit_file("../src/hilbert.c");
	emit_file("../src/imag.c");
	emit_file("../src/index.c");
	emit_file("../src/inner.c");
	emit_file("../src/integral.c");
	emit_file("../src/inv.c");
	emit_file("../src/is.c");
	emit_file("../src/isprime.c");
	emit_file("../src/itab.c");
	emit_file("../src/laguerre.c");
	emit_file("../src/lcm.c");
	emit_file("../src/leading.c");
	emit_file("../src/legendre.c");
	emit_file("../src/log.c");
	emit_file("../src/mag.c");
	emit_file("../src/mod.c");
	emit_file("../src/multiply.c");
	emit_file("../src/nroots.c");
	emit_file("../src/numerator.c");
	emit_file("../src/outer.c");
	emit_file("../src/partition.c");
	emit_file("../src/polar.c");
	emit_file("../src/pollard.c");
	emit_file("../src/power.c");
	emit_file("../src/prime.c");
	emit_file("../src/primetab.c");
	emit_file("../src/print.c");
	emit_file("../src/product.c");
	emit_file("../src/quotient.c");
	emit_file("../src/rationalize.c");
	emit_file("../src/real.c");
	emit_file("../src/rect.c");
	emit_file("../src/roots.c");
	emit_file("../src/setq.c");
	emit_file("../src/sgn.c");
	emit_file("../src/simfac.c");
	emit_file("../src/simplify.c");
	emit_file("../src/sin.c");
	emit_file("../src/sinh.c");
	emit_file("../src/sum.c");
	emit_file("../src/tan.c");
	emit_file("../src/tanh.c");
	emit_file("../src/taylor.c");
	emit_file("../src/tensor.c");
	emit_file("../src/test.c");
	emit_file("../src/transform.c");
	emit_file("../src/transpose.c");
	emit_file("../src/userfunc.c");
	emit_file("../src/zero.c");

	emit_file("../src/core.c");
	emit_file("../src/symbol.c");
	emit_file("../src/stack.c");
	emit_file("../src/bignum.c");

	return 0;
}

void
emit_file(char *filename)
{
	int line = 1, state = 0;
	FILE *f;
	static char str[1000];

	emit_line("\n");

	f = fopen(filename, "r");

	if (f == NULL) {
		fprintf(stderr, "cannot open %s\n", filename);
		exit(1);
	}

	while (fgets(str, sizeof str, f)) {

		check_line(filename, line++, str);

		if (strcmp(str, "#include \"defs.h\"\n") == 0)
			continue;

		if (state && *str == '\n')
			continue; // skip blank lines in functions

		emit_line(str);

		if (*str == '{')
			state = 1;

		if (*str == '}')
			state = 0;
	}

	fclose(f);
}

void
check_line(char *filename, int line_number, char *line)
{
	int i, n;
	n = strlen(line);
	if (n < 1 || line[n - 1] != '\n') {
		fprintf(stderr, "file %s line %d: line error\n", filename, line_number);
		exit(1);
	}
	if (n < 2)
		return;
	// check for weird ascii
	for (i = 0; i < n; i++) {
		if (line[i] >= ' ' && line[i] < 0x7f)
			continue;
		if (line[i] == '\t' || line[i] == '\n')
			continue;
		fprintf(stderr, "%s line %d: ascii error\n", filename, line_number);
		exit(1);
	}
	// check leading space
	for (i = 0; i < n; i++) {
		if (line[i] > ' ')
			break;
		if (line[i] == '\t')
			continue;
		fprintf(stderr, "%s line %d: leading space\n", filename, line_number);
		// exit(1);
	}
	// check trailing space
	if (line[n - 2] <= ' ') {
		fprintf(stderr, "%s line %d: trailing space\n", filename, line_number);
		exit(1);
	}
}

void
emit_line(char *line)
{
	if (prev == '\n' && *line == '\n')
		return; // don't print more than one blank line in a row
	printf("%s", line);
	prev = *line;
}
