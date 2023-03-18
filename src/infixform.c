void
eval_infixform(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	outbuf_init();
	infixform_expr(p1);

	push_string(outbuf);
}

// for tty mode and debugging

void
print_infixform(struct atom *p)
{
	outbuf_init();
	infixform_expr(p);
	outbuf_puts("\n");
	printbuf(outbuf, BLACK);
}

void
infixform_subexpr(struct atom *p)
{
	outbuf_puts("(");
	infixform_expr(p);
	outbuf_puts(")");
}

void
infixform_expr(struct atom *p)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p))))
		outbuf_puts("-");
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
			outbuf_puts(" - ");
		else
			outbuf_puts(" + ");
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
		outbuf_puts(" / ");
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
		outbuf_puts(" "); // space in between factors
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
			outbuf_puts(" "); // space in between factors

		if (isrational(q)) {
			s = mstr(q->u.q.a);
			outbuf_puts(s);
			continue;
		}

		infixform_factor(q);
	}

	if (k == 0)
		outbuf_puts("1");
}

void
infixform_denominators(struct atom *p)
{
	int k, n;
	char *s;
	struct atom *q;

	n = count_denominators(p);

	if (n > 1)
		outbuf_puts("(");

	k = 0;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (++k > 1)
			outbuf_puts(" "); // space in between factors

		if (isrational(q)) {
			s = mstr(q->u.q.b);
			outbuf_puts(s);
			continue;
		}

		if (isminusone(caddr(q))) {
			q = cadr(q);
			infixform_factor(q);
		} else {
			infixform_base(cadr(q));
			outbuf_puts("^");
			infixform_numeric_exponent(caddr(q)); // sign is not emitted
		}
	}

	if (n > 1)
		outbuf_puts(")");
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
			outbuf_puts("exp(1)");
		else
			outbuf_puts(printname(p));
		return;
	}

	if (isstr(p)) {
		outbuf_puts(p->u.str);
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
		outbuf_puts("d");
		infixform_arglist(p);
		return;
	}

	if (car(p) == symbol(SETQ)) {
		infixform_expr(cadr(p));
		outbuf_puts(" = ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		infixform_expr(cadr(p));
		outbuf_puts(" == ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		infixform_expr(cadr(p));
		outbuf_puts(" >= ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		infixform_expr(cadr(p));
		outbuf_puts(" > ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		infixform_expr(cadr(p));
		outbuf_puts(" <= ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		infixform_expr(cadr(p));
		outbuf_puts(" < ");
		infixform_expr(caddr(p));
		return;
	}

	// other function

	if (iscons(p)) {
		infixform_base(car(p));
		infixform_arglist(p);
		return;
	}

	outbuf_puts(" ? ");
}

void
infixform_power(struct atom *p)
{
	if (cadr(p) == symbol(EXP1)) {
		outbuf_puts("exp(");
		infixform_expr(caddr(p));
		outbuf_puts(")");
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(J_LOWER)))) {
			outbuf_puts("j");
			return;
		}
		if (isimaginaryunit(get_binding(symbol(I_LOWER)))) {
			outbuf_puts("i");
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		infixform_reciprocal(p);
		return;
	}

	infixform_base(cadr(p));

	outbuf_puts("^");

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
	outbuf_puts("1 / "); // numerator
	if (isminusone(caddr(p))) {
		p = cadr(p);
		infixform_factor(p);
	} else {
		infixform_base(cadr(p));
		outbuf_puts("^");
		infixform_numeric_exponent(caddr(p)); // sign is not emitted
	}
}

void
infixform_factorial(struct atom *p)
{
	infixform_base(cadr(p));
	outbuf_puts("!");
}

void
infixform_index(struct atom *p)
{
	infixform_base(cadr(p));
	outbuf_puts("[");
	p = cddr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			outbuf_puts(",");
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	outbuf_puts("]");
}

void
infixform_arglist(struct atom *p)
{
	outbuf_puts("(");
	p = cdr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			outbuf_puts(",");
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	outbuf_puts(")");
}

// sign is not emitted

void
infixform_rational(struct atom *p)
{
	char *s;

	s = mstr(p->u.q.a);
	outbuf_puts(s);

	s = mstr(p->u.q.b);

	if (strcmp(s, "1") == 0)
		return;

	outbuf_puts("/");

	outbuf_puts(s);
}

// sign is not emitted

void
infixform_double(struct atom *p)
{
	char *s;

	snprintf(strbuf, STRBUFLEN, "%g", fabs(p->u.d));

	s = strbuf;

	while (*s && *s != 'E' && *s != 'e')
		outbuf_putc(*s++);

	if (!*s)
		return;

	s++;

	outbuf_puts(" 10^");

	if (*s == '-') {
		outbuf_puts("(-");
		s++;
		while (*s == '0')
			s++; // skip leading zeroes
		outbuf_puts(s);
		outbuf_puts(")");
	} else {
		if (*s == '+')
			s++;
		while (*s == '0')
			s++; // skip leading zeroes
		outbuf_puts(s);
	}
}

void
infixform_base(struct atom *p)
{
	if (isnum(p))
		infixform_numeric_base(p);
	else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		infixform_subexpr(p);
	else
		infixform_expr(p);
}

void
infixform_numeric_base(struct atom *p)
{
	if (isposint(p))
		infixform_rational(p);
	else
		infixform_subexpr(p);
}

// sign is not emitted

void
infixform_numeric_exponent(struct atom *p)
{
	if (isdouble(p)) {
		outbuf_puts("(");
		infixform_double(p);
		outbuf_puts(")");
		return;
	}

	if (isinteger(p)) {
		infixform_rational(p);
		return;
	}

	outbuf_puts("(");
	infixform_rational(p);
	outbuf_puts(")");
}

void
infixform_tensor(struct atom *p)
{
	infixform_tensor_nib(p, 0, 0);
}

void
infixform_tensor_nib(struct atom *p, int d, int k)
{
	int i, n, span;

	if (d == p->u.tensor->ndim) {
		infixform_expr(p->u.tensor->elem[k]);
		return;
	}

	span = 1;

	n = p->u.tensor->ndim;

	for (i = d + 1; i < n; i++)
		span *= p->u.tensor->dim[i];

	outbuf_puts("(");

	n = p->u.tensor->dim[d];

	for (i = 0; i < n; i++) {

		infixform_tensor_nib(p, d + 1, k);

		if (i < n - 1)
			outbuf_puts(",");

		k += span;
	}

	outbuf_puts(")");
}
