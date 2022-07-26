void
eval_roots(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		eval();
	} else
		push_symbol(X_LOWER);

	roots();
}

void
roots(void)
{
	int h, i, j, n;
	struct atom *A, *C, *LIST, *P, *X;

	X = pop();
	P = pop();

	h = tos;

	coeffs(P, X); // put coeffs on stack

	// check coeffs

	for (i = h; i < tos; i++)
		if (!isrational(stack[i]))
			stop("roots: coeffs");

	LIST = symbol(NIL);

	while (tos - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue; // coeff of monomial is zero

		// divide through by C

		for (i = h; i < tos; i++) {
			push(stack[i]);
			push(C);
			divide();
			stack[i] = pop();
		}

		push_integer(1); // new leading coeff

		if (findroot(h) == 0)
			break; // no root found

		A = pop(); // root

		push(A);
		push(LIST);
		cons(); // prepend A to LIST
		LIST = pop();

		reduce(h, A); // divide by X - A
	}

	tos = h; // pop all

	n = length(LIST);

	if (n == 0) {
		push_symbol(NIL); // no roots found
		return;
	}

	if (n == 1) {
		push(car(LIST)); // one root found
		return;
	}

	for (i = 0; i < n; i++) {
		push(car(LIST));
		LIST = cdr(LIST);
	}

	sort(n); // sort roots

	// eliminate repeated roots

	for (i = 0; i < n - 1; i++)
		if (equal(stack[i], stack[i + 1])) {
			for (j = i + i; j < n - 1; j++)
				stack[j] = stack[j + 1];
			i--;
			n--;
		}

	if (n == 1) {
		tos = h + 1;
		return;
	}

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A->u.tensor->elem[i] = stack[h + i];

	tos = h; // pop all

	push(A);
}

// polynomial coefficients are on the stack

int
findroot(int h)
{
	int i, j, k, m, n;
	struct atom *C, *T, *X;

	C = stack[h]; // constant term

	if (iszero(C)) {
		push_integer(0); // root is zero
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

// divide by X - A

void
reduce(int h, struct atom *A)
{
	int i, t;

	t = tos - 1;

	for (i = t; i > h; i--) {
		push(A);
		push(stack[i]);
		multiply();
		push(stack[i - 1]);
		add();
		stack[i - 1] = pop();
	}

	if (!iszero(stack[h]))
		stop("roots: residual error");

	for (i = h; i < t; i++)
		stack[i] = stack[i + 1];

	pop(); // one less coeff on stack
}

// push all divisors of n

void
divisors(int n)
{
	int h, i, k;

	h = tos;

	factor_int(n);

	k = tos;

	// contruct divisors by recursive descent

	push_integer(1);

	divisors_nib(h, k);

	// move

	n = tos - k;

	for (i = 0; i < n; i++)
		stack[h + i] = stack[k + i];

	tos = h + n; // pop all
}

//	Generate all divisors for a factored number
//
//	Input:		Factor pairs on stack (base, expo)
//
//			h	first pair
//
//			k	just past last pair
//
//	Output:		Divisors on stack
//
//	For example, the number 12 (= 2^2 3^1) has 6 divisors:
//
//	1, 2, 3, 4, 6, 12

void
divisors_nib(int h, int k)
{
	int i, n;
	struct atom *ACCUM, *BASE, *EXPO;

	if (h == k)
		return;

	ACCUM = pop();

	BASE = stack[h + 0];
	EXPO = stack[h + 1];

	push(EXPO);
	n = pop_integer();

	for (i = 0; i <= n; i++) {
		push(ACCUM);
		push(BASE);
		push_integer(i);
		power();
		multiply();
		divisors_nib(h + 2, k);
	}
}
