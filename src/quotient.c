void
eval_quotient(struct atom *p1)
{
	push(cadr(p1));
	eval();

	push(caddr(p1));
	eval();

	p1 = cdddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		eval();
	} else
		push_symbol(X_LOWER);

	quotient();
}

void
quotient(void)
{
	int i, k, m, n, p, q;
	struct atom *P, *Q, *T, *X, *Y;

	X = pop();
	Q = pop();
	P = pop();

	p = tos;
	factorpoly_coeffs(P, X);
	m = tos - p - 1; // m is degree of dividend

	q = tos;
	factorpoly_coeffs(Q, X);
	n = tos - q - 1; // n is degree of divisor

	k = m - n;

	Y = zero;

	while (k >= 0) {

		push(stack[p + m]);
		push(stack[q + n]);
		divide();
		T = pop();

		for (i = 0; i <= n; i++) {
			push(stack[p + k + i]);
			push(stack[q + i]);
			push(T);
			multiply();
			subtract();
			stack[p + k + i] = pop();
		}

		push(Y);
		push(T);
		push(X);
		push_integer(k);
		power();
		multiply();
		add();
		Y = pop();

		m--;
		k--;
	}

	tos = p; // pop all

	push(Y);
}
