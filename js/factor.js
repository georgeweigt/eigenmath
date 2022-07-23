// factors N or N^M where N and M are rational numbers, returns factors on stack

function
factor()
{
	var numer, denom;
	var INPUT, BASE, EXPO;

	INPUT = pop();

	if (car(INPUT) == symbol(POWER)) {

		BASE = cadr(INPUT);
		EXPO = caddr(INPUT);

		if (!isrational(BASE) || !isrational(EXPO)) {
			push(INPUT); // cannot factor
			return;
		}

		if (isminusone(BASE)) {
			push(INPUT); // -1 to the M
			return;
		}

		if (isnegativenumber(BASE)) {
			push_symbol(POWER);
			push_integer(-1);
			push(EXPO);
			list(3); // leave on stack
		}

		numer = BASE.a;
		denom = BASE.b;

		if (!bignum_equal(numer, 1))
			factor_bignum(numer, EXPO);

		if (!bignum_equal(denom, 1)) {
			// flip sign of exponent
			push(EXPO);
			negate();
			EXPO = pop();
			factor_bignum(denom, EXPO);
		}

		return;
	}

	if (!isrational(INPUT) || iszero(INPUT) || isplusone(INPUT) || isminusone(INPUT)) {
		push(INPUT);
		return;
	}

	if (isnegativenumber(INPUT))
		push_integer(-1);

	numer = INPUT.a;
	denom = INPUT.b;

	if (!bignum_equal(numer, 1))
		factor_bignum(numer, one);

	if (!bignum_equal(denom, 1))
		factor_bignum(denom, minusone);
}
