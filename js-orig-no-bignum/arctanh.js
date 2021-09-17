function
arctanh()
{
	var p1 = pop();

	if (isplusone(p1) || isminusone(p1)) {
		push_symbol(ARCTANH);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p1)) {
		push_double(Math.atanh(p1.d));
		return;
	}

	// arctanh(z) = 1/2 (log(1 + z) - log(1 - z))

	if (isdoublez(p1)) {
		push_double(1.0);
		push(p1);
		add();
		log();
		push_double(1.0);
		push(p1);
		subtract();
		log();
		subtract();
		push_double(0.5);
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// arctanh(-x) = -arctanh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		arctanh();
		negate();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCTANH);
	push(p1);
	list(2);
}
