function
cosh()
{
	var p1 = pop();

	// cosh(z) = 1/2 (exp(z) + exp(-z))

	if (isdouble(p1) || isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		exp();
		push(p1);
		negate();
		exp();
		add();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(1);
		return;
	}

	// cosh(-x) = cosh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		cosh();
		return;
	}

	if (car(p1) == symbol(ARCCOSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(COSH);
	push(p1);
	list(2);
}
