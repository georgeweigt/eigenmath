function
reduce_radical_rational(h, COEFF)
{
	var i, k, n, p1, p2, NUMER, DENOM, BASE, EXPO;

	if (isplusone(COEFF) || isminusone(COEFF))
		return COEFF; // COEFF has no factors, no cancellation is possible

	push(COEFF);
	absfunc();
	p1 = pop();

	push(p1);
	numerator();
	NUMER = pop();

	push(p1);
	denominator();
	DENOM = pop();

	k = 0;

	n = stack.length;

	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (!isradical(p1))
			continue;
		BASE = cadr(p1);
		EXPO = caddr(p1);
		if (isnegativenumber(EXPO)) {
			mod_integers(NUMER, BASE);
			p2 = pop();
			if (iszero(p2)) {
				push(NUMER);
				push(BASE);
				divide();
				NUMER = pop();
				push_symbol(POWER);
				push(BASE);
				push_integer(1);
				push(EXPO);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		} else {
			mod_integers(DENOM, BASE);
			p2 = pop();
			if (iszero(p2)) {
				push(DENOM);
				push(BASE);
				divide();
				DENOM = pop();
				push_symbol(POWER);
				push(BASE);
				push_integer(-1);
				push(EXPO);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		}
	}

	if (k) {
		push(NUMER);
		push(DENOM);
		divide();
		if (isnegativenumber(COEFF))
			negate();
		COEFF = pop();
	}

	return COEFF;
}
