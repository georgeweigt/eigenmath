// n is a bignum, exponent is a rational

function
factor_number(n, exponent)
{
	var d, k, m, t;

	if (n.length > 2 || (n.length == 2 && n[1] > 255)) {
		// more than 32 bits
		push_rational_number(1, bignum_copy(n), bignum_int(1));
		if (!equaln(exponent, 1)) {
			push_symbol(POWER);
			swap();
			push(exponent);
			list(3);
		}
		return;
	}

	if (n.length == 1)
		n = n[0];
	else
		n = BIGM * n[1] + n[0];

	for (k = 0; k < 10000; k++) {

		d = primetab[k];

		if (n / d < d)
			break; // n is 1 or prime

		m = 0;

		while (n % d == 0) {
			n /= d;
			m++;
		}

		if (m == 0)
			continue;

		push_integer(d);

		push_integer(m);
		push(exponent);
		multiply();
		t = pop();

		if (!isplusone(t)) {
			push_symbol(POWER);
			swap();
			push(t);
			list(3);
		}
	}

	if (n > 1) {
		push_integer(n);
		if (!isplusone(exponent)) {
			push_symbol(POWER);
			swap();
			push(exponent);
			list(3);
		}
	}
}
