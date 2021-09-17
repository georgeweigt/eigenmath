function
arctan_numbers(X, Y)
{
	var x, y, T;

	if (iszero(X) && iszero(Y)) {
		push_symbol(ARCTAN);
		push_integer(0);
		push_integer(0);
		list(3);
		return;
	}

	if (isnum(X) && isnum(Y) && (isdouble(X) || isdouble(Y))) {
		push(X);
		x = pop_double();
		push(Y);
		y = pop_double();
		push_double(Math.atan2(y, x));
		return;
	}

	// X and Y are rational numbers

	if (iszero(Y)) {
		if (isnegativenumber(X)) {
			push_integer(-1);
			push_symbol(PI);
			multiply();
		} else
			push_integer(0);
		return;
	}

	if (iszero(X)) {
		if (isnegativenumber(Y))
			push_rational(-1, 2);
		else
			push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	// convert fractions to integers

	push(Y);
	push(X);
	divide();
	abs();
	T = pop();

	push(T);
	numerator();
	if (isnegativenumber(Y))
		negate();
	Y = pop();

	push(T);
	denominator();
	if (isnegativenumber(X))
		negate();
	X = pop();

	// compare numerators and denominators, ignore signs

	if (bignum_cmp(X.a, Y.a) != 0 || bignum_cmp(X.b, Y.b) != 0) {
		// not equal
		push_symbol(ARCTAN);
		push(Y);
		push(X);
		list(3);
		return;
	}

	// X == Y (modulo sign)

	if (isnegativenumber(X))
		if (isnegativenumber(Y))
			push_rational(-3, 4);
		else
			push_rational(3, 4);
	else
		if (isnegativenumber(Y))
			push_rational(-1, 4);
		else
			push_rational(1, 4);

	push_symbol(PI);
	multiply();
}
