#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH "../src/"

int prev = '\n';

void emit_file(char *);
void emit_line(char *);
void check_line(char *, int, char *);

int
main()
{
	system("cat preamble.h");

	emit_file(PATH "defs1.h");
	emit_file(PATH "prototypes.h");

	emit_file(PATH "globals.c");
	emit_file(PATH "main.c");
	emit_file(PATH "run.c");
	emit_file(PATH "scan.c");
	emit_file(PATH "eval.c");
	emit_file(PATH "display.c");

	emit_file(PATH "abs.c");
	emit_file(PATH "add.c");
	emit_file(PATH "adj.c");
	emit_file(PATH "arccos.c");
	emit_file(PATH "arccosh.c");
	emit_file(PATH "arcsin.c");
	emit_file(PATH "arcsinh.c");
	emit_file(PATH "arctan.c");
	emit_file(PATH "arctanh.c");
	emit_file(PATH "arg.c");
	emit_file(PATH "atomize.c");
	emit_file(PATH "bake.c");
	emit_file(PATH "besselj.c");
	emit_file(PATH "bessely.c");
	emit_file(PATH "binomial.c");
	emit_file(PATH "ceiling.c");
	emit_file(PATH "choose.c");
	emit_file(PATH "circexp.c");
	emit_file(PATH "clock.c");
	emit_file(PATH "coeff.c");
	emit_file(PATH "cofactor.c");
	emit_file(PATH "conj.c");
	emit_file(PATH "contract.c");
	emit_file(PATH "cos.c");
	emit_file(PATH "cosh.c");
	emit_file(PATH "defint.c");
	emit_file(PATH "degree.c");
	emit_file(PATH "denominator.c");
	emit_file(PATH "derivative.c");
	emit_file(PATH "det.c");
	emit_file(PATH "distill.c");
	emit_file(PATH "divisors.c");
	emit_file(PATH "eigen.c");
	emit_file(PATH "erf.c");
	emit_file(PATH "erfc.c");
	emit_file(PATH "exp.c");
	emit_file(PATH "expand.c");
	emit_file(PATH "expcos.c");
	emit_file(PATH "expsin.c");
	emit_file(PATH "factor.c");
	emit_file(PATH "factorial.c");
	emit_file(PATH "factorpoly.c");
	emit_file(PATH "factors.c");
	emit_file(PATH "filter.c");
	emit_file(PATH "float.c");
	emit_file(PATH "floor.c");
	emit_file(PATH "for.c");
	emit_file(PATH "gcd.c");
	emit_file(PATH "guess.c");
	emit_file(PATH "hermite.c");
	emit_file(PATH "hilbert.c");
	emit_file(PATH "imag.c");
	emit_file(PATH "index.c");
	emit_file(PATH "inner.c");
	emit_file(PATH "integral.c");
	emit_file(PATH "inv.c");
	emit_file(PATH "is.c");
	emit_file(PATH "isprime.c");
	emit_file(PATH "itab.c");
	emit_file(PATH "laguerre.c");
	emit_file(PATH "lcm.c");
	emit_file(PATH "leading.c");
	emit_file(PATH "legendre.c");
	emit_file(PATH "log.c");
	emit_file(PATH "mag.c");
	emit_file(PATH "mod.c");
	emit_file(PATH "multiply.c");
	emit_file(PATH "nroots.c");
	emit_file(PATH "numerator.c");
	emit_file(PATH "outer.c");
	emit_file(PATH "partition.c");
	emit_file(PATH "polar.c");
	emit_file(PATH "pollard.c");
	emit_file(PATH "power.c");
	emit_file(PATH "prime.c");
	emit_file(PATH "primetab.c");
	emit_file(PATH "print.c");
	emit_file(PATH "product.c");
	emit_file(PATH "quotient.c");
	emit_file(PATH "rationalize.c");
	emit_file(PATH "real.c");
	emit_file(PATH "rect.c");
	emit_file(PATH "roots.c");
	emit_file(PATH "setq.c");
	emit_file(PATH "sgn.c");
	emit_file(PATH "simfac.c");
	emit_file(PATH "simplify.c");
	emit_file(PATH "sin.c");
	emit_file(PATH "sinh.c");
	emit_file(PATH "sum.c");
	emit_file(PATH "tan.c");
	emit_file(PATH "tanh.c");
	emit_file(PATH "taylor.c");
	emit_file(PATH "tensor.c");
	emit_file(PATH "test.c");
	emit_file(PATH "transform.c");
	emit_file(PATH "transpose.c");
	emit_file(PATH "userfunc.c");
	emit_file(PATH "zero.c");

	emit_file(PATH "core.c");
	emit_file(PATH "symbol.c");
	emit_file(PATH "stack.c");
	emit_file(PATH "bignum.c");

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
