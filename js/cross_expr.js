function
cross_expr(p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (cross_term(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return cross_term(p);
}

function
cross_term(p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (cross_factor(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return cross_factor(p);
}

function
cross_factor(p)
{
	if (isrational(p)) {
		if (isinteger(p))
			return 0;
		push_integer(p.b);
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
