function
power_complex_number(BASE, EXPO)
{
	var n, X, Y;

	// lisp(2+3*i) = (add 2 (multiply 3 (power -1 1/2)))

	// lisp(1+i) = (add 1 (power -1 1/2))

	// lisp(3*i) = (multiply 3 (power -1 1/2))

	// lisp(i) = (power -1 1/2)

	if (car(BASE) == symbol(ADD)) {
		X = cadr(BASE);
		if (caaddr(BASE) == symbol(MULTIPLY))
			Y = cadaddr(BASE);
		else
			Y = one;
	} else if (car(BASE) == symbol(MULTIPLY)) {
		X = zero;
		Y = cadr(BASE);
	} else {
		X = zero;
		Y = one;
	}

	if (isdouble(X) || isdouble(Y) || isdouble(EXPO)) {
		power_complex_double(BASE, EXPO, X, Y);
		return;
	}

	if (!isinteger(EXPO)) {
		power_complex_rational(BASE, EXPO, X, Y);
		return;
	}

	if (!isinteger(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	n = EXPO.a;

	if (n > 0)
		power_complex_plus(X, Y, n);
	else if (n < 0)
		power_complex_minus(X, Y, -n);
	else
		push_integer(1);
}
