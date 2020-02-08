#include "defs.h"

#define MML_MINUS "<mo prefix='true'>-</mo>"

void
eval_mathml(void)
{
	push(cadr(p1));
	eval();

	mml();

	push_string(outbuf);
}

void
mml(void)
{
	save();

	outbuf_index = 0;
	print_str("<math><mrow>");

	p1 = pop();
	mml_expr(p1);

	print_str("</mrow></math>");
	print_char('\0');

	restore();
}

void
mml_expr(struct atom *p)
{
	struct atom *q;

	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		q = car(p);
		if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q)))) {
			print_str(MML_MINUS);
			mml_term(q);
		} else
			mml_term(q);
		p = cdr(p);
		while (iscons(p)) {
			q = car(p);
			if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
				mml_mo("-");
			else
				mml_mo("+");
			mml_term(q);
			p = cdr(p);
		}
	} else {
		if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p)))) {
			print_str(MML_MINUS);
			mml_term(p);
		} else
			mml_term(p);
	}
}

void
mml_term(struct atom *p)
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
			print_str("<mfrac><mrow>");
			mml_numerators(p);
			print_str("</mrow><mrow>");
			mml_denominators(p);
			print_str("</mrow></mfrac>");
			return;
		}

		// no denominators

		p = cdr(p);

		if (isrational(car(p)) && isminusone(car(p)))
			p = cdr(p); // skip -1

		while (iscons(p)) {
			mml_factor(car(p));
			p = cdr(p);
		}
	} else
		mml_factor(p);
}

void
mml_numerators(struct atom *p)
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
				mml_mn(s);
				n++;
			}
			p = cdr(p);
			continue;
		}

		if (isdouble(q)) {
			mml_double(q);
			n++;
			p = cdr(p);
			continue;
		}

		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // printed in denominator
		}

		mml_factor(q);
		n++;
		p = cdr(p);
	}

	if (n == 0)
		mml_mn("1"); // there were no numerators
}

void
mml_denominators(struct atom *p)
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
				mml_mn(s);
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
			print_str("<msup>");
			mml_mo("(");
			print_str(MML_MINUS);
			mml_mn("1");
			mml_mo(")");
			mml_number(caddr(q)); // sign not printed
			print_str("</msup>");
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/x

		if (isminusone(caddr(q))) {
			mml_factor(cadr(q));
			n++;
			p = cdr(p);
			continue;
		}

		// default

		print_str("<msup><mrow>");
		mml_factor(cadr(q));
		print_str("</mrow><mrow>");
		mml_number(caddr(q)); // sign not printed
		print_str("</mrow></msup>");

		n++;
		p = cdr(p);
	}

	if (n == 0)
		mml_mn("1"); // there were no denominators
}

void
mml_factor(struct atom *p)
{
	if (istensor(p)) {
		mml_tensor(p);
		return;
	}

	if (isrational(p)) {
		mml_rational(p);
		return;
	}

	if (isdouble(p)) {
		mml_double(p);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		mml_subexpr(p);
		return;
	}

	if (car(p) == symbol(POWER)) {
		mml_power(p);
		return;
	}

	if (iscons(p)) {
		mml_function(p);
		return;
	}

	if (issymbol(p)) {
		mml_symbol(p);
		return;
	}

	if (isstr(p)) {
		print_str("<ms>");
		print_str(p->u.str);
		print_str("</ms>");
		return;
	}
}

void
mml_number(struct atom *p)
{
	if (isrational(p))
		mml_rational(p);
	else
		mml_double(p);
}

void
mml_rational(struct atom *p)
{
	char *s;

	if (MEQUAL(p->u.q.b, 1)) {
		s = mstr(p->u.q.a);
		mml_mn(s);
		return;
	}

	print_str("<mfrac>");

	s = mstr(p->u.q.a);
	mml_mn(s);

	s = mstr(p->u.q.b);
	mml_mn(s);

	print_str("</mfrac>");
}

void
mml_double(struct atom *p)
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
		mml_mn(s);
		return;
	}

	*e = '\0';

	print_str("<mrow><mn>");

	print_str(s);

	if (strchr(s, '.') == NULL)
		print_str(".0");

	print_str("</mn>");

	mml_mo("&times;");

	print_str("<msup>");
	mml_mn("10");
	print_str("<mrow>");

	s = e + 1;

	if (*s == '+')
		s++;
	else if (*s == '-') {
		s++;
		print_str("<mo form='prefix'>-</mo>");
	}

	while (*s == '0')
		s++; // skip leading zeroes

	mml_mn(s);

	print_str("</mrow></msup></mrow>");
}

void
mml_power(struct atom *p)
{
	// case (-1)^x

	if (isminusone(cadr(p))) {
		mml_imaginary(p);
		return;
	}

	// case e^x

	if (cadr(p) == symbol(EXP1)) {
		mml_mi("exp");
		mml_mo("(");
		mml_expr(caddr(p));
		mml_mo(")");
		return;
	}

	// example 1/x

	if (isminusone(caddr(p))) {
		print_str("<mfrac>");
		mml_mn("1");
		print_str("<mrow>");
		mml_expr(cadr(p));
		print_str("</mrow></mfrac>");
		return;
	}

	// example 1/x^2

	if (isnegativenumber(caddr(p))) {
		print_str("<mfrac>");
		mml_mn("1");
		print_str("<msup>");
		mml_factor(cadr(p));
		mml_number(caddr(p));
		print_str("</msup></mfrac>");
		return;
	}

	// default case

	print_str("<msup>");
	mml_factor(cadr(p));
	print_str("<mrow>");
	mml_expr(caddr(p));
	print_str("</mrow></msup>");
}

// case (-1)^x

void
mml_imaginary(struct atom *p)
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(binding[SYMBOL_J])) {
			mml_mi("j");
			return;
		}
		if (isimaginaryunit(binding[SYMBOL_I])) {
			mml_mi("i");
			return;
		}
	}

	// example (-1)^(-1/4)

	if (isnegativenumber(caddr(p))) {
		print_str("<mfrac>");
		mml_mn("1");
		print_str("<msup>");
		mml_mo("(");
		print_str(MML_MINUS);
		mml_mn("1");
		mml_mo(")");
		mml_number(caddr(p));
		print_str("</msup></mfrac>");
		return;
	}

	// default case

	print_str("<msup>");

	mml_mo("(");
	print_str(MML_MINUS);
	mml_mn("1");
	mml_mo(")");

	print_str("<mrow>");
	mml_expr(caddr(p));
	print_str("</mrow>");

	print_str("</msup>");
}

void
mml_function(struct atom *p)
{
	if (car(p) == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) { // no user func d
		mml_derivative_function(p);
		return;
	}

	if (car(p) == symbol(FACTORIAL)) {
		mml_factorial_function(p);
		return;
	}

	if (car(p) == symbol(INDEX)) {
		mml_index_function(p);
		return;
	}

	if (car(p) == symbol(SETQ)) {
		mml_expr(cadr(p));
		mml_mo("=");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		mml_expr(cadr(p));
		mml_mo("=");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		mml_expr(cadr(p));
		mml_mo("&ge;");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		mml_expr(cadr(p));
		mml_mo("&gt;");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		mml_expr(cadr(p));
		mml_mo("&le;");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		mml_expr(cadr(p));
		mml_mo("&lt");
		mml_expr(caddr(p));
		return;
	}

	// function name

	if (issymbol(car(p)))
		mml_symbol(car(p));
	else
		mml_subexpr(car(p));

	// function arglist

	mml_mo("(");
	mml_arglist(p);
	mml_mo(")");
}

void
mml_derivative_function(struct atom *p)
{
	print_str("<mi mathvariant=\"normal\">d</mi>");
	mml_mo("(");
	mml_arglist(p);
	mml_mo(")");
}

void
mml_factorial_function(struct atom *p)
{
	p = cadr(p);
	if (isposint(p) || issymbol(p))
		mml_expr(p);
	else
		mml_subexpr(p);
	mml_mo("!");
}

void
mml_index_function(struct atom *p)
{
	p = cdr(p);

	if (issymbol(car(p)))
		mml_symbol(car(p));
	else
		mml_subexpr(car(p));

	mml_mo("[");
	mml_arglist(p);
	mml_mo("]");
}

void
mml_arglist(struct atom *p)
{
	p = cdr(p);
	if (iscons(p)) {
		mml_expr(car(p));
		p = cdr(p);
		while(iscons(p)) {
			mml_mo(",");
			mml_expr(car(p));
			p = cdr(p);
		}
	}
}

void
mml_subexpr(struct atom *p)
{
	mml_mo("(");
	mml_expr(p);
	mml_mo(")");
}

void
mml_symbol(struct atom *p)
{
	int n;
	char *s;

	if (iskeyword(p) || p == symbol(AUTOEXPAND) || p == symbol(LAST) || p == symbol(TRACE) || p == symbol(TTY)) {
		mml_mi(p->u.printname);
		return;
	}

	if (p == symbol(EXP1)) {
		mml_mi("exp");
		mml_mo("(");
		mml_mn("1");
		mml_mo(")");
		return;
	}

	s = p->u.printname;
	n = mml_symbol_scan(s);

	if (strlen(s) == n) {
		mml_symbol_shipout(s, n);
		return;
	}

	// subscripted

	print_str("<msub>");

	mml_symbol_shipout(s, n);
	s += n;

	print_str("<mrow>");

	while (*s) {
		n = mml_symbol_scan(s);
		mml_symbol_shipout(s, n);
		s += n;
	}

	print_str("</mrow></msub>");
}

char *mml_greek_tab[46] = {
	"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota",
	"Kappa","Lambda","Mu","Nu","Xi","Pi","Rho","Sigma","Tau","Upsilon",
	"Phi","Chi","Psi","Omega",
	"alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota",
	"kappa","lambda","mu","nu","xi","pi","rho","sigma","tau","upsilon",
	"phi","chi","psi","omega",
};

int
mml_symbol_scan(char *s)
{
	int i, n;
	for (i = 0; i < 46; i++) {
		n = strlen(mml_greek_tab[i]);
		if (strncmp(s, mml_greek_tab[i], n) == 0)
			return n;
	}
	return 1;
}

void
mml_symbol_shipout(char *s, int n)
{
	int i;

	if (n == 1) {
		print_str("<mi>");
		print_char(*s);
		print_str("</mi>");
		return;
	}

	// greek

	if (*s >= 'A' && *s <= 'Z') {
		print_str("<mi mathvariant='normal'>&"); // upper case
		for (i = 0; i < n; i++)
			print_char(*s++);
		print_str(";</mi>");
	} else {
		print_str("<mi>&");
		for (i = 0; i < n; i++)
			print_char(*s++);
		print_str(";</mi>");
	}
}

void
mml_tensor(struct atom *p)
{
}

void
mml_mi(char *s)
{
	print_str("<mi>");
	print_str(s);
	print_str("</mi>");
}

void
mml_mn(char *s)
{
	print_str("<mn>");
	print_str(s);
	print_str("</mn>");
}

void
mml_mo(char *s)
{
	// extra <mrow> needed so any MML_MINUS does not bind to ( or ]

	if (strcmp(s, "(") == 0) {
		print_str("<mrow><mo>(</mo><mrow>");
		return;
	}

	if (strcmp(s, ")") == 0) {
		print_str("</mrow><mo>)</mo></mrow>");
		return;
	}

	if (strcmp(s, "[") == 0) {
		print_str("<mrow><mo>[</mo><mrow>");
		return;
	}

	if (strcmp(s, "]") == 0) {
		print_str("</mrow><mo>]</mo></mrow>");
		return;
	}

	print_str("<mo>");
	print_str(s);
	print_str("</mo>");
}
