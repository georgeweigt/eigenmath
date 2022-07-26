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
	int h, i, j, k, m, n;
	struct atom *A, *P, *X;

	X = pop();
	P = pop();

	h = tos;

	coeffs(P, X); // put coeffs on stack

	k = tos;

	if (k - h == 1) {
		stack[h] = symbol(NIL); // no roots
		return;
	}

	// check coeffs

	for (i = h; i < k; i++)
		if (!isrational(stack[i]))
			stop("roots: coeffs");

	m = k;

	while (k - h > 1) {

		if (findroot(h, k) == 0)
			break; // no root found

		A = stack[tos - 1]; // root

		reduce(h, k, A); // divide by X - A

		k--; // one less coeff
	}

	n = tos - m; // number of roots on stack

	if (n == 0) {
		stack[h] = symbol(NIL); // no roots
		tos = h + 1; // pop all
		return;
	}

	if (n == 1) {
		stack[h] = stack[m]; // one root
		tos = h + 1; // pop all
		return;
	}

	sort(n); // sort roots

	// eliminate repeated roots

	for (i = 0; i < n - 1; i++)
		if (equal(stack[m + i], stack[m + i + 1])) {
			for (j = i + i; j < n - 1; j++)
				stack[m + j] = stack[m + j + 1];
			i--;
			n--;
		}

	if (n == 1) {
		stack[h] = stack[m]; // one root
		tos = h + 1; // pop all
		return;
	}

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A->u.tensor->elem[i] = stack[m + i];

	tos = h; // pop all

	push(A);
}

// coefficients are on the stack

int
findroot(int h, int k)
{
	int i, j, n, p, q, r;
	struct atom *A, *C, *PA;

	C = stack[h]; // constant term

	if (iszero(C)) {
		push_integer(0); // root is zero
		return 1;
	}

	C = stack[k - 1]; // leading coeff

	// divide through by C

	for (i = h; i < k; i++) {
		push(stack[i]);
		push(C);
		divide();
		stack[i] = pop();
	}

	C = stack[h];

	p = tos;

	push(C);
	numerator();
	n = pop_integer();
	divisors(n); // push divisors of n

	q = tos;

	push(C);
	denominator();
	n = pop_integer();
	divisors(n); // push divisors of n

	r = tos;

	for (i = p; i < q; i++) {
		for (j = q; j < r; j++) {

			// try postive A

			push(stack[i]);
			push(stack[j]);
			divide();
			A = pop();

			horner(h, k, A);

			PA = pop();

			if (iszero(PA)) {
				tos = p; // pop all
				push(A);
				return 1; // root on stack
			}

			// try negative A

			push(A);
			negate();
			A = pop();

			horner(h, k, A);

			PA = pop();

			if (iszero(PA)) {
				tos = p; // pop all
				push(A);
				return 1; // root on stack
			}
		}
	}

	tos = p; // pop all

	return 0; // no root
}

// evaluate p(x) at x = A using horner's rule

void
horner(int h, int k, struct atom *A)
{
	int i;

	push(stack[k - 1]);

	for (i = k - 2; i >= h; i--) {
		push(A);
		multiply();
		push(stack[i]);
		add();
	}
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

// divide by X - A

void
reduce(int h, int k, struct atom *A)
{
	int i;

	for (i = k - 1; i > h; i--) {
		push(A);
		push(stack[i]);
		multiply();
		push(stack[i - 1]);
		add();
		stack[i - 1] = pop();
	}

	if (!iszero(stack[h]))
		stop("roots: residual error"); // not a root

	for (i = h; i < k - 1; i++)
		stack[i] = stack[i + 1];
}
