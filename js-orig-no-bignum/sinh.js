function
sinh()
{
	var p1 = pop();

	// sinh(z) = 1/2 (exp(z) - exp(-z))

	if (isdouble(p1) || isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		exp();
		push(p1);
		negate();
		exp();
		subtract();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// sinh(-x) -> -sinh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		sinh();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCSINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(SINH);
	push(p1);
	list(2);
}
