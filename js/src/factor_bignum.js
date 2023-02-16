// N is bignum, M is rational

function
factor_bignum(N, M)
{
	var h, i, n;
	var BASE, EXPO;

	// greater than 31 bits?

	if (!bignum_issmallnum(N)) {
		push_bignum(1, bignum_copy(N), bignum_int(1));
		if (isplusone(M))
			return;
		push_symbol(POWER);
		swap();
		push(M);
		list(3);
		return;
	}

	h = stack.length;

	n = bignum_smallnum(N);

	factor_int(n);

	n = (stack.length - h) / 2; // number of factors on stack

	for (i = 0; i < n; i++) {

		BASE = stack[h + 2 * i + 0];
		EXPO = stack[h + 2 * i + 1];

		push(EXPO);
		push(M);
		multiply();
		EXPO = pop();

		if (isplusone(EXPO)) {
			stack[h + i] = BASE;
			continue;
		}

		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		stack[h + i] = pop();
	}

	stack.splice(h + n); // pop all
}
