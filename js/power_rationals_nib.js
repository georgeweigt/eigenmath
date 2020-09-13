function
power_rationals_nib(BASE, EXPO) // BASE is prime number, EXPO is rational
{
	var n, q, r;

	// integer power?

	if (EXPO.b == 1) {
		a = Math.pow(BASE.a, Math.abs(EXPO.a));
		if (EXPO.a < 0)
			push_rational(1, a); // reciprocate
		else
			push_rational(a, 1);
		return;
	}

	q = Math.floor(Math.abs(EXPO.a) / EXPO.b);
	r = Math.abs(EXPO.a) % EXPO.b;

	// whole part

	if (q > 0) {

		n = Math.pow(BASE.a, q);

		if (EXPO.a < 0)
			push_rational(1, n); // reciprocate
		else
			push_rational(n, 1);
	}

	// remainder (BASE is prime hence no roots)

	if (EXPO.a < 0)
		r = -r;

	push_symbol(POWER);
	push(BASE);
	push_rational(r, EXPO.b);
	list(3);
}
