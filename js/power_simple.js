function
power_simple(BASE, EXPO) // EXPO is an integer
{
	var a, b, base, expo;

	if (isrational(BASE) && isrational(EXPO)) {

		a = Math.pow(BASE.a, Math.abs(EXPO.a));
		b = Math.pow(BASE.b, Math.abs(EXPO.a));

		if (isnegativenumber(EXPO))
			push_rational(b, a); // reciprocate
		else
			push_rational(a, b);

		return;

	}

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	push_double(Math.pow(base, expo));
}
