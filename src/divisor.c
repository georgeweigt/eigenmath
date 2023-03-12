int
divisor(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (divisor_term(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return divisor_term(p);
}

int
divisor_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (divisor_factor(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return divisor_factor(p);
}

int
divisor_factor(struct atom *p)
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
