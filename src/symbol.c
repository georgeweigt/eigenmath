#include "defs.h"

// put symbol s at index n

void
std_symbol(char *s, int n)
{
	struct atom *p;
	p = symtab + n;
	p->u.printname = strdup(s);
	if (p->u.printname == NULL)
		malloc_kaput();
}

// symbol lookup, create symbol if not found

struct atom *
usr_symbol(char *s)
{
	int i;
	struct atom *p;
	for (i = 0; i < NSYM; i++) {
		if (symtab[i].u.printname == 0)
			break;
		if (strcmp(s, symtab[i].u.printname) == 0)
			return symtab + i;
	}
	if (i == NSYM)
		stop("symbol table full");
	p = symtab + i;
	p->u.printname = strdup(s);
	if (p->u.printname == NULL)
		malloc_kaput();
	return p;
}

// get the symbol's print name

char *
get_printname(struct atom *p)
{
	if (p->k != SYM)
		stop("symbol error");
	return p->u.printname;
}

void
set_binding(struct atom *p, struct atom *b)
{
	if (p->k != SYM || p - symtab < MARK2)
		stop("reserved symbol");
	binding[p - symtab] = b;
	arglist[p - symtab] = symbol(NIL);
}

void
set_binding_and_arglist(struct atom *p, struct atom *b, struct atom *a)
{
	if (p->k != SYM || p - symtab < MARK2)
		stop("reserved symbol");
	binding[p - symtab] = b;
	arglist[p - symtab] = a;
}

struct atom *
get_binding(struct atom *p)
{
	if (p->k != SYM)
		stop("symbol error");
	return binding[p - symtab];
}

struct atom *
get_arglist(struct atom *p)
{
	if (p->k != SYM)
		stop("symbol error");
	return arglist[p - symtab];
}

// get symbol's number from ptr

int
symnum(struct atom *p)
{
	if (p->k != SYM)
		stop("symbol error");
	return (int) (p - symtab);
}

void
push_binding(struct atom *p)
{
	if (p->k != SYM)
		stop("symbol expected");
	push(binding[p - symtab]);
	push(arglist[p - symtab]);
}

void
pop_binding(struct atom *p)
{
	if (p->k != SYM)
		stop("symbol expected");
	arglist[p - symtab] = pop();
	binding[p - symtab] = pop();
}

void
init_symbol_table(void)
{
	int i;

	for (i = 0; i < NSYM; i++) {
		symtab[i].k = SYM;
		if (symtab[i].u.printname) {
			free(symtab[i].u.printname);
			symtab[i].u.printname = NULL;
		}
		binding[i] = symtab + i;
		arglist[i] = symbol(NIL);
	}

	std_symbol("abs", ABS);
	std_symbol("+", ADD);
	std_symbol("adj", ADJ);
	std_symbol("and", AND);
	std_symbol("arccos", ARCCOS);
	std_symbol("arccosh", ARCCOSH);
	std_symbol("arcsin", ARCSIN);
	std_symbol("arcsinh", ARCSINH);
	std_symbol("arctan", ARCTAN);
	std_symbol("arctanh", ARCTANH);
	std_symbol("arg", ARG);
	std_symbol("atomize", ATOMIZE);
	std_symbol("besselj", BESSELJ);
	std_symbol("bessely", BESSELY);
	std_symbol("binding", BINDING);
	std_symbol("binomial", BINOMIAL);
	std_symbol("ceiling", CEILING);
	std_symbol("check", CHECK);
	std_symbol("choose", CHOOSE);
	std_symbol("circexp", CIRCEXP);
	std_symbol("clear", CLEAR);
	std_symbol("clock", CLOCK);
	std_symbol("coeff", COEFF);
	std_symbol("cofactor", COFACTOR);
	std_symbol("conj", CONJ);
	std_symbol("contract", CONTRACT);
	std_symbol("cos", COS);
	std_symbol("cosh", COSH);
	std_symbol("defint", DEFINT);
	std_symbol("deg", DEGREE);
	std_symbol("denominator", DENOMINATOR);
	std_symbol("derivative", DERIVATIVE);
	std_symbol("det", DET);
	std_symbol("dim", DIM);
	std_symbol("divisors", DIVISORS);
	std_symbol("do", DO);
	std_symbol("dot", DOT);
	std_symbol("draw", DRAW);
	std_symbol("erf", ERF);
	std_symbol("erfc", ERFC);
	std_symbol("eigen", EIGEN);
	std_symbol("eigenval", EIGENVAL);
	std_symbol("eigenvec", EIGENVEC);
	std_symbol("eval", EVAL);
	std_symbol("exit", EXIT);
	std_symbol("exp", EXP);
	std_symbol("expand", EXPAND);
	std_symbol("expcos", EXPCOS);
	std_symbol("expcosh", EXPCOSH);
	std_symbol("expsin", EXPSIN);
	std_symbol("expsinh", EXPSINH);
	std_symbol("exptan", EXPTAN);
	std_symbol("exptanh", EXPTANH);
	std_symbol("factor", FACTOR);
	std_symbol("factorial", FACTORIAL);
	std_symbol("filter", FILTER);
	std_symbol("float", FLOATF);
	std_symbol("floor", FLOOR);
	std_symbol("for", FOR);
	std_symbol("gcd", GCD);
	std_symbol("hermite", HERMITE);
	std_symbol("hilbert", HILBERT);
	std_symbol("imag", IMAG);
	std_symbol("[", INDEX);
	std_symbol("inner", INNER);
	std_symbol("integral", INTEGRAL);
	std_symbol("inv", INV);
	std_symbol("isprime", ISPRIME);
	std_symbol("laguerre", LAGUERRE);
	std_symbol("latex", LATEX);
	std_symbol("lcm", LCM);
	std_symbol("leading", LEADING);
	std_symbol("legendre", LEGENDRE);
	std_symbol("lisp", LISP);
	std_symbol("log", LOG);
	std_symbol("mag", MAG);
	std_symbol("mathml", MATHML);
	std_symbol("mod", MOD);
	std_symbol("*", MULTIPLY);
	std_symbol("not", NOT);
	std_symbol("nroots", NROOTS);
	std_symbol("number", NUMBER);
	std_symbol("numerator", NUMERATOR);
	std_symbol("or", OR);
	std_symbol("outer", OUTER);
	std_symbol("polar", POLAR);
	std_symbol("^", POWER);
	std_symbol("prime", PRIME);
	std_symbol("print", PRINT);
	std_symbol("product", PRODUCT);
	std_symbol("quote", QUOTE);
	std_symbol("quotient", QUOTIENT);
	std_symbol("rank", RANK);
	std_symbol("rationalize", RATIONALIZE);
	std_symbol("real", REAL);
	std_symbol("rect", RECTF);
	std_symbol("roots", ROOTS);
	std_symbol("run", RUN);
	std_symbol("=", SETQ);
	std_symbol("sgn", SGN);
	std_symbol("simplify", SIMPLIFY);
	std_symbol("sin", SIN);
	std_symbol("sinh", SINH);
	std_symbol("sqrt", SQRT);
	std_symbol("status", STATUS);
	std_symbol("stop", STOP);
	std_symbol("string", STRING);
	std_symbol("subst", SUBST);
	std_symbol("sum", SUM);
	std_symbol("tan", TAN);
	std_symbol("tanh", TANH);
	std_symbol("taylor", TAYLOR);
	std_symbol("test", TEST);
	std_symbol("test.eq", TESTEQ);
	std_symbol("test.ge", TESTGE);
	std_symbol("test.gt", TESTGT);
	std_symbol("test.le", TESTLE);
	std_symbol("test.lt", TESTLT);
	std_symbol("transpose", TRANSPOSE);
	std_symbol("unit", UNIT);
	std_symbol("zero", ZERO);

	std_symbol("$", MARK1);

	std_symbol(".e", EXP1);
	std_symbol("nil", NIL);
	std_symbol("pi", PI);

	std_symbol("$", MARK2);

	std_symbol("$a", METAA); // must be distinct so they sort correctly
	std_symbol("$b", METAB);
	std_symbol("$x", METAX);
	std_symbol("$X", SPECX);

	std_symbol("autoexpand", AUTOEXPAND);
	std_symbol("last", LAST);
	std_symbol("trace", TRACE);
	std_symbol("tty", TTY);

	std_symbol(".", MARK3);

	std_symbol("d", SYMBOL_D);
	std_symbol("i", SYMBOL_I);
	std_symbol("j", SYMBOL_J);
	std_symbol("s", SYMBOL_S);
	std_symbol("t", SYMBOL_T);
	std_symbol("x", SYMBOL_X);
	std_symbol("y", SYMBOL_Y);
	std_symbol("z", SYMBOL_Z);
}
