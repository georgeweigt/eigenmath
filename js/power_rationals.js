function
power_rationals(BASE, EXPO)
{
	var d, h, i, n, base, expo, p1, COEFF;

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
		p1 = stack[i];
		if (car(p1) == symbol(POWER)) {
			BASE = cadr(p1);
			EXPO = caddr(p1);
			power_rationals_nib(BASE, EXPO);
			stack[i] = pop(); // 1 or 2 factors on stack, fill hole with tos
		}
	}

	// combine numbers (leaves radicals on stack)

	COEFF = one;

	n = stack.length;

	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (isnum(p1)) {
			push(COEFF);
			push(p1);
			multiply();
			COEFF = pop();
			stack.splice(i, 1);
			i--;
			n--;
		}
	}

	// float radicals if COEFF is double (can happen due to auto conversion of rational to double)

	n = stack.length;

	if (isdouble(COEFF) && n - h > 0) {
		d = COEFF.d;
		n = stack.length;
		for (i = h; i < n; i++) {
			p1 = stack[i];
			BASE = cadr(p1);
			EXPO = caddr(p1);
			base = BASE.a / BASE.b;
			expo = EXPO.a / EXPO.b;
			d *= Math.pow(base, expo);
		}
		stack.splice(h); // pop all
		push_double(d);
		COEFF = pop();
	}

	// finalize

	n = stack.length - h;

	if (n == 0 || !isplusone(COEFF) || isdouble(COEFF)) {
		push(COEFF);
		n++;
	}

	if (n == 1)
		return;

	sort_factors(h);
	list(n);
	push_symbol(MULTIPLY);
	swap();
	cons();
}
