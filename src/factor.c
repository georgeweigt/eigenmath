void
eval_factor(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		eval();
	} else
		push_symbol(X_LOWER);

	// factorpoly();

	pop();
	pop();

	push_symbol(NIL);
}

#if 0
void
factorpoly(void)
{
	int h, i, n;
	struct atom *A, *C, *F, *P, *X;

	X = pop();
	P = pop();

	h = tos;

	coeffs(P, X); // put coeffs on stack

	F = one;

	while (tos - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue;

		// F = C * F

		push(F);
		push(C);
		multiply_noexpand();
		F = pop();

		// divide through by C

		for (i = h; i < tos; i++) {
			push(stack[i]);
			push(C);
			divide();
			stack[i] = pop();
		}

		push_integer(1); // leading coeff

		if (findroot(h) == 0)
			break;

		A = pop();

		// F = F * (X - A)

		push(F);
		push(X);
		push(A);
		subtract();
		multiply_noexpand();
		F = pop();

		reduce(h, A); // divide by X - A
	}

	n = tos - h;

	if (n == 0) {
		push(F);
		return;
	}

	// remainder

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		push(X);
		push_integer(i);
		power();
		multiply();
		stack[h + i] = pop();
	}

	add_terms(n);

	push(F);
	multiply_noexpand();
}
#endif
