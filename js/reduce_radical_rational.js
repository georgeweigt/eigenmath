function
reduce_radical_factors(h, COEF)
{
	var i, j, k, n, p1, p2, NUMER, DENOM, BASE1, EXPO1;

	if (isplusone(COEF) || isminusone(COEF))
		return; // COEF has no factors, no cancellation is possible

	push(COEF);
	abs();
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
		BASE1 = cadr(p1);
		EXPO1 = caddr(p1);
		if (isnegativenumber(EXPO1)) {
			mod_integers(NUMER, BASE1);
			p2 = pop();
			if (iszero(p2)) {
				push(NUMER);
				push(BASE1);
				divide();
				NUMER = pop();
				push_symbol(POWER);
				push(BASE1);
				push_integer(1);
				push(EXPO1);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		} else {
			mod_integers(DENOM, BASE1);
			p2 = pop();
			if (iszero(p2)) {
				push(DENOM);
				push(BASE1);
				divide();
				DENOM = pop();
				push_symbol(POWER);
				push(BASE1);
				push_integer(-1);
				push(EXPO1);
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
		if (isnegativenumber(COEF))
			negate();
		COEF = pop();
	}

	return COEF;
}
