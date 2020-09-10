// BASE is an integer, EXPO is an integer or rational number

function
power_rationals_nib(BASE, EXPO)
{
	var a;

	// integer power?

	if (EXPO.b == 1) {
		a = Math.pow(BASE.a, Math.abs(EXPO.a));
		if (EXPO.a < 0)
			push_rational(1, a); // reciprocate
		else
			push_rational(a, 1);
		return;
	}

	// evaluate whole part

	if (Math.abs(EXPO.a) > EXPO.b) {

		a = Math.floor(Math.abs(EXPO.a) / EXPO.b)
		a = Math.pow(BASE.a, a);

		if (EXPO.a < 0)
			push_rational(1, a); // reciprocate
		else
			push_rational(a, 1);

		// remainder

		if (EXPO.a < 0)
			a = -Math.abs(EXPO.a) % EXPO.b;
		else
			a = Math.abs(EXPO.a) % EXPO.b;
	} else
		a = EXPO.a;

	// BASE is a prime number from factor_factor() hence no numerical roots

	push_symbol(POWER);
	push(BASE);
	push_rational(a, EXPO.b);
	list(3);
}
