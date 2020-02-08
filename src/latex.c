#include "defs.h"

void
eval_latex(void)
{
	push(cadr(p1));
	eval();

	latex();

	push_string(outbuf);
}

void
latex(void)
{
	save();

	outbuf_index = 0;
	print_str("\\begin{equation}\n");

	p1 = pop();
	latex_expr(p1);

	print_str("\n\\end{equation}");
	print_char('\0');

	restore();
}

void
latex_expr(struct atom *p)
{
	struct atom *q;

	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		q = car(p);
		if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
			print_str("-");
		latex_term(q);
		p = cdr(p);
		while (iscons(p)) {
			q = car(p);
			if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
				print_str("-");
			else
				print_str("+");
			latex_term(q);
			p = cdr(p);
		}
	} else {
		if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p))))
			print_str("-");
		latex_term(p);
	}
}

void
latex_term(struct atom *p)
{
	struct atom *q, *t;

	if (car(p) == symbol(MULTIPLY)) {

		// any denominators?

		q = cdr(p);
		while (iscons(q)) {
			t = car(q);
			if (car(t) == symbol(POWER) && isnegativenumber(caddr(t)))
				break;
			q = cdr(q);
		}

		if (iscons(q)) {
			print_str("\\frac{");
			latex_numerators(p);
			print_str("}{");
			latex_denominators(p);
			print_str("}");
			return;
		}

		// no denominators

		p = cdr(p);

		if (isrational(car(p)) && isminusone(car(p)))
			p = cdr(p); // skip -1

		while (iscons(p)) {
			latex_factor(car(p));
			p = cdr(p);
		}
	} else
		latex_factor(p);
}

void
latex_numerators(struct atom *p)
{
	int n = 0;
	char *s;
	struct atom *q;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);

		if (isrational(q)) {
			if (!MEQUAL(q->u.q.a, 1)) {
				s = mstr(q->u.q.a);
				print_str(s);
				n++;
			}
			p = cdr(p);
			continue;
		}

		if (isdouble(q)) {
			latex_double(q);
			n++;
			p = cdr(p);
			continue;
		}

		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // printed in denominator
		}

		latex_factor(q);
		n++;
		p = cdr(p);
	}

	if (n == 0)
		print_str("1"); // there were no numerators
}

void
latex_denominators(struct atom *p)
{
	int n = 0;
	char *s;
	struct atom *q;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);

		if (isrational(q)) {
			if (!MEQUAL(q->u.q.b, 1)) {
				s = mstr(q->u.q.b);
				print_str(s);
				n++;
			}
			p = cdr(p);
			continue;
		}

		if (car(q) != symbol(POWER) || !isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // printed in numerator
		}

		// example (-1)^(-1/4)

		if (isminusone(cadr(q))) {
			print_str("(-1)^{");
			latex_number(caddr(q)); // sign not printed
			print_str("}");
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/x

		if (isminusone(caddr(q))) {
			latex_factor(cadr(q));
			n++;
			p = cdr(p);
			continue;
		}

		// default

		latex_factor(cadr(q));
		print_str("^{");
		latex_number(caddr(q)); // sign not printed
		print_str("}");

		n++;
		p = cdr(p);
	}

	if (n == 0)
		print_str("1"); // there were no denominators
}

void
latex_factor(struct atom *p)
{
	if (istensor(p)) {
		latex_tensor(p);
		return;
	}

	if (isnum(p)) {
		latex_number(p);
		return;
	}

	if (issymbol(p)) {
		latex_symbol(p);
		return;
	}

	if (isstr(p)) {
		print_str("\\,\\text{");
		print_str(p->u.str);
		print_str("}");
		return;
	}

	if (car(p) == symbol(POWER)) {
		latex_power(p);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		latex_subexpr(p);
		return;
	}

	if (iscons(p)) {
		latex_function(p);
		return;
	}
}

void
latex_number(struct atom *p)
{
	if (isrational(p))
		latex_rational(p);
	else
		latex_double(p);
}

void
latex_rational(struct atom *p)
{
	char *s;

	if (MEQUAL(p->u.q.b, 1)) {
		s = mstr(p->u.q.a);
		print_str(s);
		return;
	}

	print_str("\\frac{");

	s = mstr(p->u.q.a);
	print_str(s);

	print_str("}{");

	s = mstr(p->u.q.b);
	print_str(s);

	print_str("}");
}

void
latex_double(struct atom *p)
{
	char *e, *s;

	sprintf(tbuf, "%g", p->u.d);

	s = tbuf;

	if (*s == '-')
		s++;

	e = strchr(s, 'e');

	if (e == NULL)
		e = strchr(s, 'E');

	if (e == NULL) {
		if (strchr(s, '.') ==  NULL)
			strcat(s, ".0");
		print_str(s);
		return;
	}

	*e = '\0';

	print_str(s);

	if (strchr(s, '.') == NULL)
		print_str(".0");

	print_str("\\times10^{");

	s = e + 1;

	if (*s == '+')
		s++;
	else if (*s == '-') {
		s++;
		print_str("-");
	}

	while (*s == '0')
		s++; // skip leading zeroes

	print_str(s);

	print_str("}");
}

void
latex_power(struct atom *p)
{
	// case (-1)^x

	if (isminusone(cadr(p))) {
		latex_imaginary(p);
		return;
	}

	// case e^x

	if (cadr(p) == symbol(EXP1)) {
		print_str("\\exp\\left(");
		latex_expr(caddr(p));
		print_str("\\right)");
		return;
	}

	// example 1/x

	if (isminusone(caddr(p))) {
		print_str("\\frac{1}{");
		latex_expr(cadr(p));
		print_str("}");
		return;
	}

	// example 1/x^2

	if (isnegativenumber(caddr(p))) {
		print_str("\\frac{1}{");
		latex_factor(cadr(p));
		print_str("^{");
		latex_number(caddr(p));
		print_str("}}");
		return;
	}

	// default case

	latex_factor(cadr(p));
	print_str("^{");
	latex_expr(caddr(p));
	print_str("}");
}

// case (-1)^x

void
latex_imaginary(struct atom *p)
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(binding[SYMBOL_J])) {
			print_str("j");
			return;
		}
		if (isimaginaryunit(binding[SYMBOL_I])) {
			print_str("i");
			return;
		}
	}

	// example (-1)^(-1/4)

	if (isnegativenumber(caddr(p))) {
		print_str("\\frac{1}{(-1)^{");
		latex_number(caddr(p));
		print_str("}}");
		return;
	}

	// default case

	print_str("(-1)^{");
	latex_expr(caddr(p));
	print_str("}");
}

void
latex_function(struct atom *p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		print_str("\\operatorname{d}\\left(");
		latex_arglist(p);
		print_str("\\right)");
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			latex_expr(p);
		else
			latex_subexpr(p);
		print_str("!");
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			latex_symbol(car(p));
		else
			latex_subexpr(car(p));
		print_str("\\left[");
		latex_arglist(p);
		print_str("\\right]");
		return;
	}

	if (car(p) == symbol(SETQ)) {
		latex_expr(cadr(p));
		print_str("=");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		latex_expr(cadr(p));
		print_str("=");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		latex_expr(cadr(p));
		print_str("\\geq");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		latex_expr(cadr(p));
		print_str(">");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		latex_expr(cadr(p));
		print_str("\\leq");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		latex_expr(cadr(p));
		print_str("<");
		latex_expr(caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		latex_symbol(car(p));
	else
		latex_subexpr(car(p));
	print_str("\\left(");
	latex_arglist(p);
	print_str("\\right)");
}

void
latex_arglist(struct atom *p)
{
	p = cdr(p);
	if (iscons(p)) {
		latex_expr(car(p));
		p = cdr(p);
		while(iscons(p)) {
			print_str(",");
			latex_expr(car(p));
			p = cdr(p);
		}
	}
}

void
latex_subexpr(struct atom *p)
{
	print_str("\\left(");
	latex_expr(p);
	print_str("\\right)");
}

void
latex_symbol(struct atom *p)
{
	int n;
	char *s;

	if (iskeyword(p) || p == symbol(AUTOEXPAND) || p == symbol(LAST) || p == symbol(TRACE) || p == symbol(TTY)) {
		print_str("\\operatorname{");
		print_str(p->u.printname);
		print_str("}");
		return;
	}

	if (p == symbol(EXP1)) {
		print_str("\\exp(1)");
		return;
	}

	s = p->u.printname;
	n = latex_symbol_scan(s);

	if (strlen(s) == n) {
		latex_symbol_shipout(s, n);
		return;
	}

	// symbol has subscript

	latex_symbol_shipout(s, n);
	s += n;

	print_str("_{");

	while (*s) {
		n = latex_symbol_scan(s);
		latex_symbol_shipout(s, n);
		s += n;
	}

	print_str("}");
}

char *latex_greek_tab[46] = {
	"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota",
	"Kappa","Lambda","Mu","Nu","Xi","Pi","Rho","Sigma","Tau","Upsilon",
	"Phi","Chi","Psi","Omega",
	"alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota",
	"kappa","lambda","mu","nu","xi","pi","rho","sigma","tau","upsilon",
	"phi","chi","psi","omega",
};

int
latex_symbol_scan(char *s)
{
	int i, n;
	for (i = 0; i < 46; i++) {
		n = strlen(latex_greek_tab[i]);
		if (strncmp(s, latex_greek_tab[i], n) == 0)
			return n;
	}
	return 1;
}

void
latex_symbol_shipout(char *s, int n)
{
	int i;

	if (n == 1) {
		print_char(*s);
		return;
	}

	// greek

	print_str("\\");

	for (i = 0; i < n; i++)
		print_char(*s++);

	print_str(" ");
}

void
latex_tensor(struct atom *p)
{
	int i, n, k = 0;
	struct tensor *t;

	t = p->u.tensor;

	// if odd dimension then vector

	if (t->ndim % 2 == 1) {
		print_str("\\begin{pmatrix}");
		n = t->dim[0];
		for (i = 0; i < n; i++) {
			latex_tensor_matrix(t, 1, &k);
			if (i < n - 1)
				print_str("\\cr "); // row separator
		}
		print_str("\\end{pmatrix}");
	} else
		latex_tensor_matrix(t, 0, &k);
}

void
latex_tensor_matrix(struct tensor *t, int d, int *k)
{
	int i, j, ni, nj;

	if (d == t->ndim) {
		latex_expr(t->elem[*k]);
		*k = *k + 1;
		return;
	}

	ni = t->dim[d];
	nj = t->dim[d + 1];

	print_str("\\begin{pmatrix}");

	for (i = 0; i < ni; i++) {
		for (j = 0; j < nj; j++) {
			latex_tensor_matrix(t, d + 2, k);
			if (j < nj - 1)
				print_str(" & "); // column separator
		}
		if (i < ni - 1)
			print_str("\\cr "); // row separator
	}

	print_str("\\end{pmatrix}");
}

char *begin_document_str =
"\\documentclass[12pt]{article}\n"
"\\usepackage{amsmath,amsfonts,amssymb}\n"
"\% change margins\n"
"\\addtolength{\\oddsidemargin}{-.875in}\n"
"\\addtolength{\\evensidemargin}{-.875in}\n"
"\\addtolength{\\textwidth}{1.75in}\n"
"\\addtolength{\\topmargin}{-.875in}\n"
"\\addtolength{\\textheight}{1.75in}\n"
"\\begin{document}\n\n";

char *end_document_str = "\\end{document}\n";

void
begin_document(void)
{
	ffputs(begin_document_str);
}

void
end_document(void)
{
	ffputs(end_document_str);
}
