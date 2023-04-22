void
eval_rationalize(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	rationalize();
}

void
rationalize(void)
{
	int i, n;
	struct atom *p0, *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			rationalize();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	p2 = one;

	while (find_divisor(p1)) {
		p0 = pop();
		push(p0);
		push(p1);
		cancel_factor();
		p1 = pop();
		push(p0);
		push(p2);
		multiply_noexpand();
		p2 = pop();
	}

	push(p1);
	push(p2);
	reciprocate();
	multiply_noexpand();
}

// returns 1 with divisor on stack, otherwise returns 0

int
find_divisor(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (find_divisor_term(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return find_divisor_term(p);
}

int
find_divisor_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (find_divisor_factor(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return find_divisor_factor(p);
}

int
find_divisor_factor(struct atom *p)
{
	if (isinteger(p))
		return 0;

	if (isrational(p)) {
		push(p);
		denominator();
		return 1;
	}

	if (car(p) == symbol(POWER) && !isminusone(cadr(p)) && isnegativeterm(caddr(p))) {
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
cancel_factor(void)
{
	int h;
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

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
