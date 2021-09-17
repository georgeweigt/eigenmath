// BASE is a sum of terms

function
power_sum(BASE, EXPO)
{
	var h, i, n, p3, p4;

	if (iszero(EXPO)) {
		push_integer(1);
		return;
	}

	if (expanding == 0 || !isnum(EXPO) || isnegativenumber(EXPO) || isfraction(EXPO) || (isdouble(EXPO) && EXPO.d != Math.floor(EXPO.d))) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	push(EXPO);
	n = pop_integer();

	// square the sum first (prevents infinite loop through multiply)

	h = stack.length;

	p3 = cdr(BASE);

	while (iscons(p3)) {
		p4 = cdr(BASE);
		while (iscons(p4)) {
			push(car(p3));
			push(car(p4));
			multiply();
			p4 = cdr(p4);
		}
		p3 = cdr(p3);
	}

	add_terms(stack.length - h);

	// continue up to power n

	for (i = 2; i < n; i++) {
		push(BASE);
		multiply();
	}
}
