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

		if (numer < 0) {
			push_symbol(POWER);
			push_integer(-1);
			push(EXPO);
			list(3); // leave on stack
			numer = -numer;
		}

		if (numer != 1)
			factor_number(numer, EXPO);

		if (denom != 1) {
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

	if (numer < 0) {
		push_integer(-1);
		numer = -numer;
	}

	if (numer != 1)
		factor_number(numer, one);

	if (denom != 1)
		factor_number(denom, minusone);
}
