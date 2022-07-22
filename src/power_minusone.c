// power -1 to EXPO

void
power_minusone(struct atom *EXPO)
{
	// optimization for i

	if (isequalq(EXPO, 1, 2)) {
		push(imaginaryunit);
		return;
	}

	// root is an odd number?

	if (isrational(EXPO) && EXPO->u.q.b[0] & 1) {
		if (EXPO->u.q.a[0] & 1)
			push_integer(-1);
		else
			push_integer(1);
		return;
	}

	if (isrational(EXPO)) {
		normalize_clock_rational(EXPO);
		return;
	}

	if (isdouble(EXPO)) {
		normalize_clock_double(EXPO);
		rect();
		return;
	}

	push_symbol(POWER);
	push_integer(-1);
	push(EXPO);
	list(3);
}

void
normalize_clock_rational(struct atom *EXPO)
{
	int n;
	struct atom *R;

	// R = EXPO mod 2

	push(EXPO);
	push_integer(2);
	modfunc();
	R = pop();

	// convert negative rotation to positive

	if (R->sign == MMINUS) {
		push(R);
		push_integer(2);
		add();
		R = pop();
	}

	push(R);
	push_integer(2);
	multiply();
	floorfunc();
	n = pop_integer(); // number of 90 degree turns

	push(R);
	push_integer(n);
	push_rational(-1, 2);
	multiply();
	add();
	R = pop(); // remainder

	switch (n) {

	case 0:
		if (iszero(R))
			push_integer(1);
		else {
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			list(3);
		}
		break;

	case 1:
		if (iszero(R))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			push_rational(-1, 2);
			add();
			list(3);
			list(3);
		}
		break;

	case 2:
		if (iszero(R))
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (iszero(R)) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			push_rational(-1, 2);
			add();
			list(3);
		}
		break;
	}
}

void
normalize_clock_double(struct atom *EXPO)
{
	double expo, n, r;

	expo = EXPO->u.d;

	// expo = expo mod 2

	expo = fmod(expo, 2.0);

	// convert negative rotation to positive

	if (expo < 0.0)
		expo += 2.0;

	n = floor(2.0 * expo); // number of 90 degree turns

	r = expo - n / 2.0; // remainder

	switch ((int) n) {

	case 0:
		if (r == 0.0)
			push_integer(1);
		else {
			push_symbol(POWER);
			push_integer(-1);
			push_double(r);
			list(3);
		}
		break;

	case 1:
		if (r == 0.0)
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push_double(r - 0.5);
			list(3);
			list(3);
		}
		break;

	case 2:
		if (r == 0.0)
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push_double(r);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (r == 0.0) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(POWER);
			push_integer(-1);
			push_double(r - 0.5);
			list(3);
		}
		break;
	}
}
