function
arcsinh()
{
	var p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.asinh(p1.d));
		return;
	}

	// arcsinh(z) = log(z + (z^2 + 1)^(1/2))

	if (isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(1.0);
		add();
		push_rational(1, 2);
		power();
		push(p1);
		add();
		log();
		return;
	}

	if (iszero(p1)) {
		push(p1);
		return;
	}

	// arcsinh(-x) = -arcsinh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		arcsinh();
		negate();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCSINH);
	push(p1);
	list(2);
}
