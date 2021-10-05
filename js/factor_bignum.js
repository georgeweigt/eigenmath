// N is bignum, EXPO is rational

function
factor_bignum(N, EXPO)
{
	var d, k, m, n, t;

	n = bignum_uint32(N);

	if (n == null) {
		// more than 32 bits
		push_bignum(1, bignum_copy(N), bignum_int(1));
		if (!isplusone(EXPO)) {
			push_symbol(POWER);
			swap();
			push(EXPO);
			list(3);
		}
		return;
	}

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
		push(EXPO);
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
		if (!isplusone(EXPO)) {
			push_symbol(POWER);
			swap();
			push(EXPO);
			list(3);
		}
	}
}
