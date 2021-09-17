function
push_rational(a, b)
{
	var sign;

	// FIXME call push_rational_number()

	if (b == 0)
		stopf("divide by zero");

	if (a == 0)
		b = 1;

	if (b < 0) {
		a = -a;
		b = -b;
	}

	if (a < 0)
		sign = -1;
	else
		sign = 1;

	if (Math.abs(a) < MAXINT && b < MAXINT)
		push({sign:sign, a:a, b:b});
	else
		push_double(a / b);
}
