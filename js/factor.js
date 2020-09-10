function
factor()
{
	var numer, denom, FARG, BASE, EXPO;

	FARG = pop();

	if (car(FARG) == symbol(POWER)) {

		BASE = cadr(FARG);
		EXPO = caddr(FARG);

		if (!isrational(BASE) || !isrational(EXPO)) {
			push(FARG); // cannot factor
			return;
		}

		if (equaln(BASE, -1)) {
			push(FARG); // -1 to the M
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

	if (!isrational(FARG) || iszero(FARG) || isplusone(FARG) || isminusone(FARG)) {
		push(FARG);
		return;
	}

	numer = FARG.a;
	denom = FARG.b;

	if (numer < 0) {
		push_integer(-1);
		numer = -numer;
	}

	if (numer != 1)
		factor_number(numer, one);

	if (denom != 1)
		factor_number(denom, minusone);
}
