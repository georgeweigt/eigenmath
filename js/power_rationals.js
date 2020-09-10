// BASE is positive

function
power_rationals(BASE, EXPO)
{
	var a, b, i, h, n, coeff, p1;

	// if EXPO is -1 then return reciprocal of BASE

	if (isminusone(EXPO)) {
		push_rational(BASE.b, BASE.a); // reciprocate
		return;
	}

	// if EXPO is integer then return BASE ^ EXPO

	if (isinteger(EXPO)) {
		a = Math.pow(BASE.a, Math.abs(EXPO.a));
		b = Math.pow(BASE.b, Math.abs(EXPO.a));
		if (isnegativenumber(EXPO))
			push_rational(b, a); // reciprocate
		else
			push_rational(a, b);
		return;
	}

	h = stack.length;

	// put factors on stack

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);

	factor();

	// normalize factors

	n = stack.length;

	for (i = h; i < n; i++) {
		p3 = stack[i];
		if (car(p3) == symbol(POWER)) {
			BASE = cadr(p3);
			EXPO = caddr(p3);
			power_rationals_nib(BASE, EXPO);
			stack[i] = pop(); // 1 or 2 factors on stack, fill hole with tos
		}
	}

	// multiply rationals

	coeff = one;

	n = stack.length;

	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (isnum(p1)) {
			push(coeff);
			push(p1);
			multiply();
			coeff = pop();
			stack.splice(i);
			i--;
			n--;
		}
	}

	// finalize

	if (!isplusone(coeff))
		push(coeff);

	n = stack.length - h;

	if (n > 1) {
		sort_factors(h);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons();
	}
}
