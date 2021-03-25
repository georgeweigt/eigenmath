#include "defs.h"

void
eval_infixform(void)
{
	push(cadr(p1));
	eval();
	p1 = pop();

	outbuf_index = 0;
	infixform(p1);
	print_char('\0');

	push_string(outbuf);
}

void
print_infixform(struct atom *p)
{
	outbuf_index = 0;
	infixform(p);
	print_char('\n');
	print_char('\0');
	printbuf(outbuf, BLACK);
}

void
infixform(struct atom *p)
{
	if (car(p) == symbol(SETQ)) {
		infixform_expr(cadr(p));
		print_str(" = ");
		infixform_expr(caddr(p));
	} else
		infixform_expr(p);
}

void
infixform_subexpr(struct atom *p)
{
	print_char('(');
	infixform_expr(p);
	print_char(')');
}

void
infixform_expr(struct atom *p)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p))))
		print_char('-');
	if (car(p) == symbol(ADD))
		infixform_expr_nib(p);
	else
		infixform_term(p);
}

void
infixform_expr_nib(struct atom *p)
{
	infixform_term(cadr(p));
	p = cddr(p);
	while (iscons(p)) {
		if (isnegativeterm(car(p)))
			print_str(" - ");
		else
			print_str(" + ");
		infixform_term(car(p));
		p = cdr(p);
	}
}

void
infixform_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY))
		infixform_term_nib(p);
	else
		infixform_factor(p);
}

void
infixform_term_nib(struct atom *p)
{
	if (find_denominator(p)) {
		infixform_numerators(p);
		print_str(" / ");
		infixform_denominators(p);
		return;
	}

	// no denominators

	p = cdr(p);

	if (isminusone(car(p)))
		p = cdr(p); // sign already emitted

	infixform_factor(car(p));

	p = cdr(p);

	while (iscons(p)) {
		print_char(' '); // space in between factors
		infixform_factor(car(p));
		p = cdr(p);
	}
}

void
infixform_numerators(struct atom *p)
{
	int k;
	char *s;
	struct atom *q;

	k = 0;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isnumerator(q))
			continue;

		if (++k > 1)
			print_char(' '); // space in between factors

		if (isrational(q)) {
			s = mstr(q->u.q.a);
			print_str(s);
			continue;
		}

		infixform_factor(q);
	}

	if (k == 0)
		print_char('1');
}

void
infixform_denominators(struct atom *p)
{
	int k, n;
	char *s;
	struct atom *q;

	n = count_denominators(p);

	if (n > 1)
		print_char('(');

	k = 0;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (++k > 1)
			print_char(' '); // space in between factors

		if (isrational(q)) {
			s = mstr(q->u.q.b);
			print_str(s);
			continue;
		}

		if (isminusone(caddr(q))) {
			q = cadr(q);
			infixform_factor(q);
		} else {
			infixform_base(cadr(q));
			print_char('^');
			infixform_numeric_exponent(caddr(q)); // sign is not emitted
		}
	}

	if (n > 1)
		print_char(')');
}

void
infixform_factor(struct atom *p)
{
	if (isrational(p)) {
		infixform_rational(p);
		return;
	}

	if (isdouble(p)) {
		infixform_double(p);
		return;
	}

	if (issymbol(p)) {
		if (p == symbol(EXP1))
			print_str("exp(1)");
		else
			print_str(printname(p));
		return;
	}

	if (isstr(p)) {
		print_str(p->u.str);
		return;
	}

	if (istensor(p)) {
		infixform_tensor(p);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		infixform_subexpr(p);
		return;
	}

	if (car(p) == symbol(POWER)) {
		infixform_power(p);
		return;
	}

	if (car(p) == symbol(FACTORIAL)) {
		infixform_factorial(p);
		return;
	}

	if (car(p) == symbol(INDEX)) {
		infixform_index(p);
		return;
	}

	// use d if for derivative if d not defined

	if (car(p) == symbol(DERIVATIVE) && get_usrfunc(symbol(D_LOWER)) == symbol(NIL)) {
		print_char('d');
		infixform_arglist(p);
		return;
	}

	// other function

	if (iscons(p)) {
		infixform_base(car(p));
		infixform_arglist(p);
		return;
	}

	print_str(" ? ");
}

void
infixform_power(struct atom *p)
{
	if (cadr(p) == symbol(EXP1)) {
		print_str("exp(");
		infixform_expr(caddr(p));
		print_char(')');
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(J_LOWER)))) {
			print_char('j');
			return;
		}
		if (isimaginaryunit(get_binding(symbol(I_LOWER)))) {
			print_char('i');
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		infixform_reciprocal(p);
		return;
	}

	infixform_base(cadr(p));

	print_char('^');

	p = caddr(p); // p now points to exponent

	if (isnum(p))
		infixform_numeric_exponent(p);
	else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		infixform_subexpr(p);
	else
		infixform_expr(p);
}

// p = y^x where x is a negative number

void
infixform_reciprocal(struct atom *p)
{
	print_str("1 / "); // numerator
	if (isminusone(caddr(p))) {
		p = cadr(p);
		infixform_factor(p);
	} else {
		infixform_base(cadr(p));
		print_char('^');
		infixform_numeric_exponent(caddr(p)); // sign is not emitted
	}
}

void
infixform_factorial(struct atom *p)
{
	infixform_base(cadr(p));
	print_char('!');
}

void
infixform_index(struct atom *p)
{
	infixform_base(cadr(p));
	print_char('[');
	p = cddr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			print_char(',');
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	print_char(']');
}

void
infixform_arglist(struct atom *p)
{
	print_char('(');
	p = cdr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			print_char(',');
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	print_char(')');
}

// sign is not emitted

void
infixform_rational(struct atom *p)
{
	char *s;

	s = mstr(p->u.q.a);
	print_str(s);

	s = mstr(p->u.q.b);

	if (strcmp(s, "1") == 0)
		return;

	print_char('/');

	print_str(s);
}

// sign is not emitted

void
infixform_double(struct atom *p)
{
	char buf[24], *s;

	sprintf(buf, "%g", fabs(p->u.d));

	s = buf;

	while (*s && *s != 'E' && *s != 'e')
		print_char(*s++);

	if (!*s)
		return;

	s++;

	print_str(" 10^");

	if (*s == '-') {
		print_str("(-");
		s++;
		while (*s == '0')
			s++; // skip leading zeroes
		print_str(s);
		print_char(')');
	} else {
		if (*s == '+')
			s++;
		while (*s == '0')
			s++; // skip leading zeroes
		print_str(s);
	}
}

void
infixform_base(struct atom *p)
{
	if (isnum(p))
		infixform_numeric_token(p);
	else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		infixform_subexpr(p);
	else
		infixform_expr(p);
}

// p is rational or double

void
infixform_numeric_token(struct atom *p)
{
	char buf[24], *s;

	if (isdouble(p)) {
		if (p->u.d < 0.0) {
			print_char('(');
			infixform_double(p);
			print_char(')');
			return;
		}
		sprintf(buf, "%g", p->u.d);
		if (strchr(buf, 'E') || strchr(buf, 'e')) {
			print_char('(');
			infixform_double(p);
			print_char(')');
			return;
		}
		print_str(buf);
		return;
	}

	if (isinteger(p)) {
		s = mstr(p->u.q.a);
		if (p->sign == MPLUS)
			print_str(s);
		else {
			print_str("(-");
			print_str(s);
			print_char(')');
		}
		return;
	}

	print_char('(');

	if (p->sign == MMINUS)
		print_char('-');

	s = mstr(p->u.q.a);
	print_str(s);

	print_char('/');

	s = mstr(p->u.q.b);
	print_str(s);

	print_char(')');

}

// p is rational or double, sign is not emitted

void
infixform_numeric_exponent(struct atom *p)
{
	char buf[24], *s;

	if (isdouble(p)) {
		sprintf(buf, "%g", fabs(p->u.d));
		if (strchr(buf, 'E') || strchr(buf, 'e')) {
			print_char('(');
			infixform_double(p);
			print_char(')');
		} else
			print_str(buf);
		return;
	}

	if (isinteger(p)) {
		s = mstr(p->u.q.a);
		print_str(s);
		return;
	}

	print_char('(');

	s = mstr(p->u.q.a);
	print_str(s);

	print_char('/');

	s = mstr(p->u.q.b);
	print_str(s);

	print_char(')');
}

void
infixform_tensor(struct atom *p)
{
	int k = 0;
	infixform_tensor_nib(p, 0, &k);
}

void
infixform_tensor_nib(struct atom *p, int j, int *k)
{
	int i;
	print_char('(');
	for (i = 0; i < p->u.tensor->dim[j]; i++) {
		if (j + 1 == p->u.tensor->ndim) {
			infixform_expr(p->u.tensor->elem[*k]);
			*k = *k + 1;
		} else
			infixform_tensor_nib(p, j + 1, k);
		if (i + 1 < p->u.tensor->dim[j]) {
			print_char(',');
		}
	}
	print_char(')');
}
