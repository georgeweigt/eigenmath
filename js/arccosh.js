function
arccosh()
{
	var p1 = pop();

	if (isdouble(p1) && p1.d >= 1.0) {
		push_double(Math.acosh(p1.d));
		return;
	}

	// arccosh(z) = log(z + sqrt(z^2 - 1))

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(-1.0);
		add();
		push_rational(1, 2);
		power();
		push(p1);
		add();
		log();
		return;
	}

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCCOSH);
	push(p1);
	list(2);
}
