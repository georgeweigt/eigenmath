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
		if (X.a >= 0)
			push_integer(0);
		else
			push_integer(-1);
		push_symbol(PI);
		multiply();
		return;
	}

	if (iszero(X)) {
		if (Y.a >= 0)
			push_rational(1, 2);
		else
			push_rational(-1, 2);
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
	if (Y.a < 0)
		negate();
	Y = pop();

	push(T);
	denominator();
	if (X.a < 0)
		negate();
	X = pop();

	// compare numerators and denominators, ignore signs

	if (Math.abs(X.a) != Math.abs(Y.a) || X.b != Y.b) {
		// not equal
		push_symbol(ARCTAN);
		push(Y);
		push(X);
		list(3);
		return;
	}

	// X == Y (modulo sign)

	if (X.a >= 0)
		if (Y.a >= 0)
			push_rational(1, 4);
		else
			push_rational(-1, 4);
	else
		if (Y.a >= 0)
			push_rational(3, 4);
		else
			push_rational(-3, 4);

	push_symbol(PI);
	multiply();
}
