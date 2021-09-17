function
combine_terms_nib(i, j)
{
	var coeff1, coeff2, denorm, p1, p2;

	p1 = stack[i];
	p2 = stack[j];

	if (iszero(p2))
		return 1;

	if (iszero(p1)) {
		stack[i] = p2;
		return 1;
	}

	if (isnum(p1) && isnum(p2)) {
		add_numbers(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (isnum(p1) || isnum(p2))
		return 0; // cannot add number and something else

	coeff1 = one;
	coeff2 = one;

	denorm = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		denorm = 1;
		if (isnum(car(p1))) {
			coeff1 = car(p1);
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				denorm = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		if (isnum(car(p2))) {
			coeff2 = car(p2);
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL))
				p2 = car(p2);
		}
	}

	if (!equal(p1, p2))
		return 0;

	add_numbers(coeff1, coeff2);

	coeff1 = pop();

	if (iszero(coeff1)) {
		stack[i] = coeff1;
		return 1;
	}

	if (isplusone(coeff1) && !isdouble(coeff1)) {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(p1);
			cons();
		} else
			push(p1);
	} else {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1);
			cons();
			cons();
		} else {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1);
			list(3);
		}
	}

	stack[i] = pop();

	return 1;
}
