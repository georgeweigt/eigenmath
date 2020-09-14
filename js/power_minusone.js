// convert (-1)^(a/b) to c (-1)^x where c=1 or c=-1 and -1/2 < x <= 1/2
//
// r = a mod b (remainder of a/b)
//
// q = (a/b - r) mod 2
//
// s = sign of a/b
//
// case			c	x
//
// s=1 q=0 r/b <= 1/2	1	r/b
// s=1 q=0 r/b > 1/2	-1	-(b - r)/b
//
// s=1 q=1 r/b <= 1/2	-1	r/b
// s=1 q=1 r/b > 1/2	1	-(b - r)/b
//
// s=-1 q=0 r/b < 1/2	1	-r/b
// s=-1 q=0 r/b >= 1/2	-1	(b - r)/b
//
// s=-1 q=1 r/b < 1/2	-1	-r/b
// s=-1 q=1 r/b >= 1/2	1	(b - r)/b
//
// q=1 flips sign of c
//
// s=-1 flips sign of x

function
power_minusone(BASE, EXPO)
{
	var a, b, c, q, r, s, t, theta, x, y;

	if (!isnum(EXPO)) {
		push_symbol(POWER);
		push_integer(-1);
		push(EXPO);
		list(3);
		if (isdouble(BASE)) {
			// BASE = -1.0, keep double
			push_symbol(MULTIPLY);
			swap();
			push_double(1.0);
			swap();
			list(3);
		}
		return;
	}

	if (equalq(EXPO, 1, 2)) {
		push(imaginaryunit);
		if (isdouble(BASE)) {
			// BASE = -1.0, keep double
			push_symbol(MULTIPLY);
			swap();
			push_double(1.0);
			swap();
			list(3);
		}
		return;
	}

	if (isdouble(EXPO)) {
		theta = EXPO.d * Math.PI; // pi is the polar angle for -1
		x = Math.cos(theta);
		y = Math.sin(theta);
		if (Math.abs(x) < 1e-12)
			x = 0.0;
		if (Math.abs(y) < 1e-12)
			y = 0.0;
		push_double(x);
		push_double(y);
		push(imaginaryunit);
		multiply();
		add();
		return;
	}

	// integer exponent?

	if (isinteger(EXPO)) {
		if (EXPO.a % 2 == 1) {
			// odd exponent
			if (isdouble(BASE))
				push_double(-1.0); // BASE = -1.0, keep double
			else
				push_integer(-1);
		} else {
			// even exponent
			if (isdouble(BASE))
				push_double(1.0); // BASE = -1.0, keep double
			else
				push_integer(1);
		}
		return;
	}

	a = Math.abs(EXPO.a);
	b = Math.abs(EXPO.b);

	q = Math.floor(a / b);
	r = a % b;
	t = r + r;

	if (t < b) {
		a = r;
		c = 1;
		s = 1;
	}

	if (t == b) {
		if (EXPO.a >= 0) {
			a = r;
			c = 1;
			s = 1;
		} else {
			a = b - r;
			c = -1;
			s = -1;
		}
	}

	if (t > b) {
		a = b - r;
		c = -1;
		s = -1;
	}

	if (q % 2 == 1)
		c *= -1;

	if (EXPO.a < 0)
		s *= -1;

	push_symbol(POWER);
	push_integer(-1);

	if (s == 1)
		push_rational(a, b);
	else
		push_rational(-a, b);

	list(3);

	if (c == -1) {
		push_symbol(MULTIPLY);
		swap();
		if (isdouble(BASE))
			push_double(-1.0); // BASE = -1.0, keep double
		else
			push_integer(-1);
		swap();
		list(3);
	} else if (isdouble(BASE)) {
		// BASE = -1.0, keep double
		push_symbol(MULTIPLY);
		swap();
		push_double(1.0);
		swap();
		list(3);
	}
}
