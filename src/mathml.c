#include "defs.h"

#define MML_MINUS "<mo rspace='0'>-</mo>"
#define MML_MINUS_1 "<mo>(</mo><mo rspace='0'>-</mo><mn>1</mn><mo>)</mo>"
#define MML_LP "<mrow><mo>(</mo>"
#define MML_RP "<mo>)</mo></mrow>"
#define MML_LB "<mrow><mo>[</mo>"
#define MML_RB "<mo>]</mo></mrow>"

void
eval_mathml(void)
{
	push(cadr(p1));
	eval();

	mathml();

	push_string(outbuf);
}

void
mathml(void)
{
	save();
	mathml_nib();
	restore();
}

void
mathml_nib(void)
{
	outbuf_index = 0;

	p1 = pop();

	if (isstr(p1))
		mml_string(p1, 0);
	else {
		print_str("<math xmlns='http://www.w3.org/1998/Math/MathML' display='block'>");
		mml_expr(p1);
		print_str("</math>");
	}

	print_char('\0');
}

void
mml_expr(struct atom *p)
{
	struct atom *q;

	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		q = car(p);
		if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
			print_str(MML_MINUS);
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
		if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p))))
			print_str(MML_MINUS);
		mml_term(p);
	}
}

void
mml_term(struct atom *p)
{
	struct atom *q, *t;

	if (car(p) == symbol(MULTIPLY)) {

		// any denominators?

		t = cdr(p);

		while (iscons(t)) {
			q = car(t);
			if (car(q) == symbol(POWER) && isnegativenumber(caddr(q)))
				break;
			t = cdr(t);
		}

		if (iscons(t)) {
			print_str("<mfrac>");
			print_str("<mrow>");
			mml_numerators(p);
			print_str("</mrow>");
			print_str("<mrow>");
			mml_denominators(p);
			print_str("</mrow>");
			print_str("</mfrac>");
			return;
		}

		// no denominators

		p = cdr(p);
		q = car(p);

		if (isrational(q) && isminusone(q))
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
	q = car(p);

	if (isrational(q)) {
		if (!MEQUAL(q->u.q.a, 1)) {
			s = mstr(q->u.q.a); // numerator
			mml_mn(s);
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);

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
	q = car(p);

	if (isrational(q)) {
		if (!MEQUAL(q->u.q.b, 1)) {
			s = mstr(q->u.q.b); // denominator
			mml_mn(s);
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);

		if (car(q) != symbol(POWER) || !isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // not a denominator
		}

		// example (-1)^(-1/4)

		if (isminusone(cadr(q))) {
			print_str("<msup>");
			print_str("<mrow>");
			print_str(MML_MINUS_1); // (-1)
			print_str("</mrow>");
			print_str("<mrow>");
			mml_number(caddr(q)); // -1/4 (sign not printed)
			print_str("</mrow>");
			print_str("</msup>");
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/y

		if (isminusone(caddr(q))) {
			mml_factor(cadr(q)); // y
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/y^2

		print_str("<msup>");
		print_str("<mrow>");
		mml_base(cadr(q)); // y
		print_str("</mrow>");
		print_str("<mrow>");
		mml_number(caddr(q)); // -2 (sign not printed)
		print_str("</mrow>");
		print_str("</msup>");

		n++;
		p = cdr(p);
	}

	if (n == 0)
		mml_mn("1"); // there were no denominators
}

void
mml_factor(struct atom *p)
{
	switch (p->k) {

	case RATIONAL:
		mml_rational(p);
		break;

	case DOUBLE:
		mml_double(p);
		break;

	case KSYM:
	case USYM:
		mml_symbol(p);
		break;

	case STR:
		mml_string(p, 1);
		break;

	case TENSOR:
		mml_tensor(p);
		break;

	case CONS:
		if (car(p) == symbol(POWER))
			mml_power(p);
		else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY))
			mml_subexpr(p);
		else
			mml_function(p);
		break;
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
		if (strchr(s, '.') == NULL)
			strcat(s, ".0");
		mml_mn(s);
		return;
	}

	*e = '\0';

	print_str("<mn>");
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
		print_str(MML_MINUS);
	}

	while (*s == '0')
		s++; // skip leading zeroes

	mml_mn(s);

	print_str("</mrow>");
	print_str("</msup>");
}

void
mml_power(struct atom *p)
{
	// (-1)^x

	if (isminusone(cadr(p))) {
		mml_imaginary(p);
		return;
	}

	// e^x

	if (cadr(p) == symbol(EXP1)) {
		mml_mi("exp");
		print_str(MML_LP);
		mml_expr(caddr(p)); // x
		print_str(MML_RP);
		return;
	}

	// example 1/y

	if (isminusone(caddr(p))) {
		print_str("<mfrac>");
		mml_mn("1"); // 1
		print_str("<mrow>");
		mml_expr(cadr(p)); // y
		print_str("</mrow>");
		print_str("</mfrac>");
		return;
	}

	// example 1/y^2

	if (isnegativenumber(caddr(p))) {
		print_str("<mfrac>");
		mml_mn("1"); // 1
		print_str("<msup>");
		print_str("<mrow>");
		mml_base(cadr(p)); // y
		print_str("</mrow>");
		print_str("<mrow>");
		mml_number(caddr(p)); // -2 (sign not printed)
		print_str("</mrow>");
		print_str("</msup>");
		print_str("</mfrac>");
		return;
	}

	// example y^x

	print_str("<msup>");
	print_str("<mrow>");
	mml_base(cadr(p)); // y
	print_str("</mrow>");
	print_str("<mrow>");
	mml_exponent(caddr(p)); // x
	print_str("</mrow>");
	print_str("</msup>");
}

void
mml_base(struct atom *p)
{
	if (isfraction(p) || isdouble(p) || car(p) == symbol(POWER))
		mml_subexpr(p);
	else
		mml_factor(p);
}

void
mml_exponent(struct atom *p)
{
	if (car(p) == symbol(POWER))
		mml_subexpr(p);
	else
		mml_factor(p);
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
		print_str("<mrow>");
		print_str(MML_MINUS_1); // (-1)
		print_str("</mrow>");
		print_str("<mrow>");
		mml_number(caddr(p)); // -1/4 (sign not printed)
		print_str("</mrow>");
		print_str("</msup>");
		print_str("</mfrac>");
		return;
	}

	// example (-1)^x

	print_str("<msup>");
	print_str("<mrow>");
	print_str(MML_MINUS_1); // (-1)
	print_str("</mrow>");
	print_str("<mrow>");
	mml_expr(caddr(p)); // x
	print_str("</mrow>");
	print_str("</msup>");
}

void
mml_function(struct atom *p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		print_str("<mi mathvariant='normal'>d</mi>");
		print_str(MML_LP);
		mml_arglist(p);
		print_str(MML_RP);
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			mml_expr(p);
		else
			mml_subexpr(p);
		mml_mo("!");
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			mml_symbol(car(p));
		else
			mml_subexpr(car(p));
		print_str(MML_LB);
		mml_arglist(p);
		print_str(MML_RB);
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

	// default

	if (issymbol(car(p)))
		mml_symbol(car(p));
	else
		mml_subexpr(car(p));

	print_str(MML_LP);
	mml_arglist(p);
	print_str(MML_RP);
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
	print_str(MML_LP);
	mml_expr(p);
	print_str(MML_RP);
}

void
mml_symbol(struct atom *p)
{
	int n;
	char *s;

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(NIL) || p == symbol(TRACE) || p == symbol(TTY)) {
		mml_mi(printname(p));
		return;
	}

	if (p == symbol(EXP1)) {
		mml_mi("exp");
		print_str(MML_LP);
		mml_mn("1");
		print_str(MML_RP);
		return;
	}

	s = printname(p);
	n = mml_symbol_scan(s);

	if ((int) strlen(s) == n) {
		mml_symbol_shipout(s, n);
		return;
	}

	// print symbol with subscript

	print_str("<msub>");

	mml_symbol_shipout(s, n);
	s += n;

	print_str("<mrow>");

	while (*s) {
		n = mml_symbol_scan(s);
		mml_symbol_shipout(s, n);
		s += n;
	}

	print_str("</mrow>");
	print_str("</msub>");
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
		n = (int) strlen(mml_greek_tab[i]);
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
	int i, n, k = 0;
	struct tensor *t;

	t = p->u.tensor;

	// if odd rank then vector

	if (t->ndim % 2 == 1) {
		print_str(MML_LP);
		print_str("<mtable>");
		n = t->dim[0];
		for (i = 0; i < n; i++) {
			print_str("<mtr><mtd>");
			mml_matrix(t, 1, &k);
			print_str("</mtd></mtr>");
		}
		print_str("</mtable>");
		print_str(MML_RP);
	} else
		mml_matrix(t, 0, &k);
}

void
mml_matrix(struct tensor *t, int d, int *k)
{
	int i, j, ni, nj;

	if (d == t->ndim) {
		mml_expr(t->elem[*k]);
		*k = *k + 1;
		return;
	}

	ni = t->dim[d];
	nj = t->dim[d + 1];

	print_str(MML_LP);
	print_str("<mtable>");

	for (i = 0; i < ni; i++) {
		print_str("<mtr>");
		for (j = 0; j < nj; j++) {
			print_str("<mtd>");
			mml_matrix(t, d + 2, k);
			print_str("</mtd>");
		}
		print_str("</mtr>");
	}

	print_str("</mtable>");
	print_str(MML_RP);
}

void
mml_string(struct atom *p, int mathmode)
{
	char *s = p->u.str;

	if (mathmode)
		print_str("<mtext>&nbsp;");

	while (*s) {
		switch (*s) {
		case '\n':
			print_str("<br>");
			break;
		case '&':
			print_str("&amp;");
			break;
		case '<':
			print_str("&lt;");
			break;
		case '>':
			print_str("&gt;");
			break;
		default:
			print_char(*s);
			break;
		}
		s++;
	}

	if (mathmode)
		print_str("</mtext>");
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
	print_str("<mo>");
	print_str(s);
	print_str("</mo>");
}
