function
multiply_numbers(p1, p2)
{
	var a, b;

	if (isrational(p1) && isrational(p2)) {
		multiply_rationals(p1, p2);
		return;
	}

	push(p1);
	a = pop_double();

	push(p2);
	b = pop_double();

	push_double(a * b);
}

function
multiply_rationals(p1, p2)
{
	var a, b, c;

	a = p1.a * p2.a;
	b = p1.b * p2.b;

	if (a == 0) {
		push_integer(0);
		return;
	}

	if (Math.abs(a) > MAXINT || b > MAXINT) {
		push_double(a / b);
		return;
	}

	if (b > 1) {
		c = gcd_integers(a, b)
		a /= c;
		b /= c;
	}

	push_rational(a, b);
}
