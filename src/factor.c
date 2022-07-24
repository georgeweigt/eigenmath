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

	factorpoly();
}

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

		if (factorpoly_root(h) == 0)
			break;

		A = pop();

		// F = F * (X - A)

		push(F);
		push(X);
		push(A);
		subtract();
		multiply_noexpand();
		F = pop();

		factorpoly_divide(h, A);

		pop(); // remove leading coeff
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

int
factorpoly_root(int h)
{
	int i, j, k, m, n;
	struct atom *C, *T, *X;

	C = stack[h]; // constant term

	if (!isrational(C))
		stop("factor");

	if (iszero(C)) {
		push_integer(0);
		return 1;
	}

	k = tos;
	push(C);
	numerator();
	n = pop_integer();
	divisors(n); // push divisors of n

	m = tos;
	push(C);
	denominator();
	n = pop_integer();
	divisors(n); // push divisors of n

	for (i = k; i < m; i++) {
		for (j = m; j < tos; j++) {

			push(stack[i]);
			push(stack[j]);
			divide();
			X = pop();

			factorpoly_eval(h, k - h, X);

			T = pop();

			if (iszero(T)) {
				tos = k; // pop all
				push(X);
				return 1;
			}

			push(X);
			negate();
			X = pop();

			factorpoly_eval(h, k - h, X);

			T = pop();

			if (iszero(T)) {
				tos = k; // pop all
				push(X);
				return 1;
			}
		}
	}

	tos = k; // pop all

	return 0; // no root
}

// divide by X - A

void
factorpoly_divide(int h, struct atom *A)
{
	int i;

	for (i = tos - 1; i > h; i--) {
		push(A);
		push(stack[i]);
		multiply();
		push(stack[i - 1]);
		add();
		stack[i - 1] = pop();
	}

	if (!iszero(stack[h]))
		stop("factor");

	for (i = h; i < tos - 1; i++)
		stack[i] = stack[i + 1];
}

// evaluate p(x) at x = X using horner's rule

void
factorpoly_eval(int h, int n, struct atom *X)
{
	int i;

	push(stack[h + n - 1]);

	for (i = n - 1; i > 0; i--) {
		push(X);
		multiply();
		push(stack[h + i - 1]);
		add();
	}
}
