// polynomial coefficients are on the stack

int
findroot(int h)
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

			findroot_eval(h, k - h, X);

			T = pop();

			if (iszero(T)) {
				tos = k; // pop all
				push(X);
				return 1;
			}

			push(X);
			negate();
			X = pop();

			findroot_eval(h, k - h, X);

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

// evaluate p(x) at x = X using horner's rule

void
findroot_eval(int h, int n, struct atom *X)
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
