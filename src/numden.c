void
numden(void)
{
	struct atom *p0, *p1, *p2;

	p1 = pop();
	p2 = one;

	while (numden_find_divisor(p1)) {
		p0 = pop();
		push(p0);
		push(p1);
		numden_cancel_factor();
		p1 = pop();
		push(p0);
		push(p2);
		multiply();
		p2 = pop();
	}

	push(p2);
	push(p1);
}

// returns 1 with divisor on stack, otherwise returns 0

int
numden_find_divisor(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (numden_find_divisor_term(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return numden_find_divisor_term(p);
}

int
numden_find_divisor_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (numden_find_divisor_factor(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return numden_find_divisor_factor(p);
}

int
numden_find_divisor_factor(struct atom *p)
{
	if (isrational(p) && !isinteger(p)) {
		push_bignum(MPLUS, mcopy(p->u.q.b), mint(1));
		return 1;
	}

	if (car(p) == symbol(POWER) && isnegativeterm(caddr(p))) {
		if (isminusone(caddr(p)))
			push(cadr(p));
		else {
			push_symbol(POWER);
			push(cadr(p));
			push(caddr(p));
			negate();
			list(3);
		}
		return 1;
	}

	return 0;
}

void
numden_cancel_factor(void)
{
	int h;
	struct atom *p1, *p2;

	p2 = pop(); // numerator
	p1 = pop(); // divisor

	// multiply term by term to ensure divisor is not distributed

	if (car(p2) == symbol(ADD)) {
		h = tos;
		p2 = cdr(p2);
		while (iscons(p2)) {
			push(p1);
			push(car(p2));
			multiply();
			p2 = cdr(p2);
		}
		add_terms(tos - h);
		return;
	}

	push(p1);
	push(p2);
	multiply();
}
