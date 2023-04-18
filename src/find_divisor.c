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
