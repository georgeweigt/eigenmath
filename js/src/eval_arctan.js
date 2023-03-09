function
eval_arctan(p1)
{
	push(cadr(p1));
	evalf();
	if (iscons(cddr(p1))) {
		push(caddr(p1));
		evalf();
	} else
		push_integer(1);
	arctan();
}

function
arctan()
{
	var i, n, X, Y, Z;

	X = pop();
	Y = pop();

	if (istensor(Y)) {
		Y = copy_tensor(Y);
		n = Y.elem.length;
		for (i = 0; i < n; i++) {
			push(Y.elem[i]);
			push(X);
			arctan();
			Y.elem[i] = pop();
		}
		push(Y);
		return;
	}

	if (isnum(X) && isnum(Y)) {
		arctan_numbers(X, Y);
		return;
	}

	// arctan(z) = -1/2 i log((i - z) / (i + z))

	if (!iszero(X) && (isdoublez(X) || isdoublez(Y))) {
		push(Y);
		push(X);
		divide();
		Z = pop();
		push_double(-0.5);
		push(imaginaryunit);
		multiply();
		push(imaginaryunit);
		push(Z);
		subtract();
		push(imaginaryunit);
		push(Z);
		add();
		divide();
		logfunc();
		multiply();
		return;
	}

	// arctan(-y,x) = -arctan(y,x)

	if (isnegativeterm(Y)) {
		push(Y);
		negate();
		push(X);
		arctan();
		negate();
		return;
	}

	if (car(Y) == symbol(TAN) && isplusone(X)) {
		push(cadr(Y)); // x of tan(x)
		return;
	}

	push_symbol(ARCTAN);
	push(Y);
	push(X);
	list(3);
}

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
		if (isnegativenumber(X))
			push_symbol(PI);
		else
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
	absfunc();
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
		if (isnegativenumber(Y)) {
			push_symbol(ARCTAN);
			push(Y);
			negate();
			push(X);
			list(3);
			negate();
		} else {
			push_symbol(ARCTAN);
			push(Y);
			push(X);
			list(3);
		}
		return;
	}

	// X = Y modulo sign

	if (isnegativenumber(X)) {
		if (isnegativenumber(Y))
			push_rational(-3, 4);
		else
			push_rational(3, 4);
	} else {
		if (isnegativenumber(Y))
			push_rational(-1, 4);
		else
			push_rational(1, 4);
	}

	push_symbol(PI);
	multiply();
}
