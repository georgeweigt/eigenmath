function
divide_rationals(p1, p2)
{
	var a, b, c;

	a = p1.a * p2.b;
	b = p1.b * p2.a;

	if (b < 0) {
		a = -a;
		b = -b;
	}

	if (a == 0) {
		push_integer(0);
		return;
	}

	if (Math.abs(a) > 1e10 || b > 1e10) {
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
