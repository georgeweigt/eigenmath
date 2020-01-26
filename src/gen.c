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

	emit_file("defs1.h");
	emit_file("prototypes.h");

	emit_file("globals.c");
	emit_file("main.c");
	emit_file("run.c");
	emit_file("scan.c");
	emit_file("eval.c");
	emit_file("display.c");

	emit_file("abs.c");
	emit_file("add.c");
	emit_file("adj.c");
	emit_file("arccos.c");
	emit_file("arccosh.c");
	emit_file("arcsin.c");
	emit_file("arcsinh.c");
	emit_file("arctan.c");
	emit_file("arctanh.c");
	emit_file("arg.c");
	emit_file("atomize.c");
	emit_file("bake.c");
	emit_file("besselj.c");
	emit_file("bessely.c");
	emit_file("binomial.c");
	emit_file("ceiling.c");
	emit_file("choose.c");
	emit_file("circexp.c");
	emit_file("clock.c");
	emit_file("coeff.c");
	emit_file("cofactor.c");
	emit_file("conj.c");
	emit_file("contract.c");
	emit_file("cos.c");
	emit_file("cosh.c");
	emit_file("defint.c");
	emit_file("degree.c");
	emit_file("denominator.c");
	emit_file("derivative.c");
	emit_file("det.c");
	emit_file("distill.c");
	emit_file("divisors.c");
	emit_file("eigen.c");
	emit_file("erf.c");
	emit_file("erfc.c");
	emit_file("exp.c");
	emit_file("expand.c");
	emit_file("expcos.c");
	emit_file("expsin.c");
	emit_file("factor.c");
	emit_file("factorial.c");
	emit_file("factorpoly.c");
	emit_file("factors.c");
	emit_file("filter.c");
	emit_file("float.c");
	emit_file("floor.c");
	emit_file("for.c");
	emit_file("gcd.c");
	emit_file("guess.c");
	emit_file("hermite.c");
	emit_file("hilbert.c");
	emit_file("imag.c");
	emit_file("index.c");
	emit_file("inner.c");
	emit_file("integral.c");
	emit_file("inv.c");
	emit_file("is.c");
	emit_file("isprime.c");
	emit_file("itab.c");
	emit_file("laguerre.c");
	emit_file("lcm.c");
	emit_file("leading.c");
	emit_file("legendre.c");
	emit_file("log.c");
	emit_file("mag.c");
	emit_file("mod.c");
	emit_file("multiply.c");
	emit_file("nroots.c");
	emit_file("numerator.c");
	emit_file("outer.c");
	emit_file("partition.c");
	emit_file("polar.c");
	emit_file("pollard.c");
	emit_file("power.c");
	emit_file("prime.c");
	emit_file("primetab.c");
	emit_file("print.c");
	emit_file("product.c");
	emit_file("quotient.c");
	emit_file("rationalize.c");
	emit_file("real.c");
	emit_file("rect.c");
	emit_file("roots.c");
	emit_file("setq.c");
	emit_file("sgn.c");
	emit_file("simfac.c");
	emit_file("simplify.c");
	emit_file("sin.c");
	emit_file("sinh.c");
	emit_file("sum.c");
	emit_file("tan.c");
	emit_file("tanh.c");
	emit_file("taylor.c");
	emit_file("tensor.c");
	emit_file("test.c");
	emit_file("transform.c");
	emit_file("transpose.c");
	emit_file("userfunc.c");
	emit_file("zero.c");

	emit_file("core.c");
	emit_file("symbol.c");
	emit_file("stack.c");
	emit_file("bignum.c");

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
