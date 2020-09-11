function
power_natural_number(BASE, EXPO)
{
	var x, y;

	// exp(x + i y) = exp(x) (cos(y) + i sin(y))

	if (isdoublez(EXPO)) {
		if (car(EXPO) == symbol(ADD)) {
			x = cadr(EXPO).d;
			y = cadaddr(EXPO).d;
		} else {
			x = 0.0;
			y = cadr(EXPO).d;
		}
		push_double(Math.exp(x));
		push_double(y);
		cos();
		push(imaginaryunit);
		push_double(y);
		sin();
		multiply();
		add();
		multiply();
		return;
	}

	// e^log(expr) -> expr

	if (car(EXPO) == symbol(LOG)) {
		push(cadr(EXPO));
		return;
	}

	if (simplify_polar_expr(EXPR))
		return;

	// none of the above

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);
}
