void
factor_bignum(uint32_t *U, struct atom *EXPO)
{
	int i, h, n;
	uint32_t u;
	struct atom *N, *M;

	// greater than 32 bits?

	if (MLENGTH(U) > 1) {
		push_bignum(MPLUS, mcopy(U), mint(1));
		if (isplusone(EXPO))
			return;
		push_symbol(POWER);
		swap();
		push(EXPO);
		list(3);
		return;
	}

	h = tos;

	u = U[0];

	factor_uint32(u);

	n = (tos - h) / 2; // number of factors on stack

	for (i = 0; i < n; i++) {

		N = stack[h + 2 * i + 0];
		M = stack[h + 2 * i + 1];

		push(M);
		push(EXPO);
		multiply();
		M = pop();

		if (!isplusone(M)) {
			push_symbol(POWER);
			push(N);
			push(M);
			list(3);
			N = pop();
		}

		stack[h + i] = N;
	}

	tos = h + n; // pop all
}
