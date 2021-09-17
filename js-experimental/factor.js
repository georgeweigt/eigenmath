function
factor()
{
	var numer, denom, INPUT, BASE, EXPO;

	INPUT = pop();

	if (car(INPUT) == symbol(POWER)) {

		BASE = cadr(INPUT);
		EXPO = caddr(INPUT);

		if (!isrational(BASE) || !isrational(EXPO)) {
			push(INPUT); // cannot factor
			return;
		}

		if (equaln(BASE, -1)) {
			push(INPUT); // -1 to the M
			return;
		}

		numer = BASE.a;
		denom = BASE.b;

		if (isnegativenumber(BASE)) {
			push_symbol(POWER);
			push_integer(-1);
			push(EXPO);
			list(3); // leave on stack
		}

		if (!bignum_equal(numer, 1))
			factor_number(numer, EXPO);

		if (!bignum_equal(denom, 1)) {
			// flip sign of exponent
			push(EXPO);
			negate();
			EXPO = pop();
			factor_number(denom, EXPO);
		}

		return;
	}

	if (!isrational(INPUT) || iszero(INPUT) || isplusone(INPUT) || isminusone(INPUT)) {
		push(INPUT);
		return;
	}

	numer = INPUT.a;
	denom = INPUT.b;

	if (isnegativenumber(INPUT))
		push_integer(-1);

	if (!bignum_equal(numer, 1))
		factor_number(numer, one);

	if (!bignum_equal(denom, 1))
		factor_number(denom, minusone);
}
