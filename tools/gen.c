#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void emit_file(char *);
void emit_line(char *);
void check_line(char *, int, char *);

int prev = '\n';

#define PATH "../src/"

char *filename[] = {
	PATH "defs1.h",
	PATH "prototypes.h",
	PATH "globals.c",
	PATH "main.c",
	PATH "run.c",
	PATH "scan.c",
	PATH "eval.c",
	PATH "display.c",
	PATH "abs.c",
	PATH "add.c",
	PATH "adj.c",
	PATH "arccos.c",
	PATH "arccosh.c",
	PATH "arcsin.c",
	PATH "arcsinh.c",
	PATH "arctan.c",
	PATH "arctanh.c",
	PATH "arg.c",
	PATH "atomize.c",
	PATH "bake.c",
	PATH "besselj.c",
	PATH "bessely.c",
	PATH "binomial.c",
	PATH "ceiling.c",
	PATH "choose.c",
	PATH "circexp.c",
	PATH "clock.c",
	PATH "coeff.c",
	PATH "cofactor.c",
	PATH "conj.c",
	PATH "contract.c",
	PATH "cos.c",
	PATH "cosh.c",
	PATH "defint.c",
	PATH "degree.c",
	PATH "denominator.c",
	PATH "derivative.c",
	PATH "det.c",
	PATH "distill.c",
	PATH "divisors.c",
	PATH "eigen.c",
	PATH "erf.c",
	PATH "erfc.c",
	PATH "exp.c",
	PATH "expand.c",
	PATH "expcos.c",
	PATH "expsin.c",
	PATH "factor.c",
	PATH "factorial.c",
	PATH "factorpoly.c",
	PATH "factors.c",
	PATH "filter.c",
	PATH "float.c",
	PATH "floor.c",
	PATH "for.c",
	PATH "gcd.c",
	PATH "guess.c",
	PATH "hermite.c",
	PATH "hilbert.c",
	PATH "imag.c",
	PATH "index.c",
	PATH "inner.c",
	PATH "integral.c",
	PATH "inv.c",
	PATH "is.c",
	PATH "isprime.c",
	PATH "itab.c",
	PATH "laguerre.c",
	PATH "lcm.c",
	PATH "leading.c",
	PATH "legendre.c",
	PATH "log.c",
	PATH "mag.c",
	PATH "mod.c",
	PATH "multiply.c",
	PATH "nroots.c",
	PATH "numerator.c",
	PATH "outer.c",
	PATH "partition.c",
	PATH "polar.c",
	PATH "pollard.c",
	PATH "power.c",
	PATH "prime.c",
	PATH "primetab.c",
	PATH "print.c",
	PATH "product.c",
	PATH "quotient.c",
	PATH "rationalize.c",
	PATH "real.c",
	PATH "rect.c",
	PATH "roots.c",
	PATH "setq.c",
	PATH "sgn.c",
	PATH "simfac.c",
	PATH "simplify.c",
	PATH "sin.c",
	PATH "sinh.c",
	PATH "sum.c",
	PATH "tan.c",
	PATH "tanh.c",
	PATH "taylor.c",
	PATH "tensor.c",
	PATH "test.c",
	PATH "transform.c",
	PATH "transpose.c",
	PATH "userfunc.c",
	PATH "zero.c",
	PATH "core.c",
	PATH "symbol.c",
	PATH "stack.c",
	PATH "bignum.c",
	NULL
};

int
main()
{
	int i = 0;
	system("cat preamble.h");
	while (filename[i])
		emit_file(filename[i++]);
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
