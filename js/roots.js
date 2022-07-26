function
roots()
{
	var h, i, j, k, n;
	var A, P, X;

	X = pop();
	P = pop();

	h = stack.length;

	coeffs(P, X); // put coeffs on stack

	k = stack.length;

	n = k - h; // number of coeffs on stack

	// check coeffs

	for (i = h; i < k; i++)
		if (!isrational(stack[i]))
			stopf("roots: coeffs");

	// divide through by leading coeff

	for (i = 0; i < n - 1; i++) {
		push(stack[h + i]);
		push(stack[h + n - 1]);
		divide();
		stack[h + i] = pop();
	}

	stack[h + n - 1] = one;

	// find roots

	while (n > 1) {

		if (findroot(h, n) == 0)
			break; // no root found

		A = stack[stack.length - 1]; // root

		reduce(h, n, A); // leading coeff is still 1

		n--;
	}

	n = stack.length - k; // number of roots on stack

	if (n == 0) {
		stack[h] = symbol(NIL); // no roots
		stack.splice(h + 1); // pop all
		return;
	}

	if (n == 1) {
		stack[h] = stack[k]; // one root
		stack.splice(h + 1); // pop all
		return;
	}

	sort(n); // sort roots

	// eliminate repeated roots

	for (i = 0; i < n - 1; i++)
		if (equal(stack[k + i], stack[k + i + 1])) {
			for (j = i + 1; j < n - 1; j++)
				stack[k + j] = stack[k + j + 1];
			i--;
			n--;
		}

	if (n == 1) {
		stack[h] = stack[k]; // one root
		stack.splice(h + 1); // pop all
		return;
	}

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A.elem[i] = stack[k + i];

	stack.splice(h); // pop all

	push(A);
}

function
findroot(h, n)
{
	var i, j, m, p, q, r;
	var A, C, PA;

	C = stack[h]; // constant term

	if (iszero(C)) {
		push_integer(0); // root is zero
		return 1;
	}

	p = stack.length;

	push(C);
	numerator();
	m = pop_integer();
	divisors(m); // push divisors of m

	q = stack.length;

	push(C);
	denominator();
	m = pop_integer();
	divisors(m); // push divisors of m

	r = stack.length;

	for (i = p; i < q; i++) {
		for (j = q; j < r; j++) {

			// try positive A

			push(stack[i]);
			push(stack[j]);
			divide();
			A = pop();

			horner(h, n, A);

			PA = pop(); // polynomial evaluated at A

			if (iszero(PA)) {
				stack.splice(p); // pop all
				push(A);
				return 1; // root on stack
			}

			// try negative A

			push(A);
			negate();
			A = pop();

			horner(h, n, A);

			PA = pop(); // polynomial evaluated at A

			if (iszero(PA)) {
				stack.splice(p); // pop all
				push(A);
				return 1; // root on stack
			}
		}
	}

	stack.splice(p); // pop all

	return 0; // no root
}

// evaluate p(x) at x = A using horner's rule

function
horner(h, n, A)
{
	var i;

	push(stack[h + n - 1]);

	for (i = n - 2; i >= 0; i--) {
		push(A);
		multiply();
		push(stack[h + i]);
		add();
	}
}

// push all divisors of n

function
divisors(n)
{
	var h, i, k;

	h = stack.length;

	factor_int(n);

	k = stack.length;

	// contruct divisors by recursive descent

	push_integer(1);

	divisors_nib(h, k);

	// move

	n = stack.length - k;

	for (i = 0; i < n; i++)
		stack[h + i] = stack[k + i];

	stack.splice(h + n); // pop all
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

function
divisors_nib(h, k)
{
	var i, n;
	var ACCUM, BASE, EXPO;

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

function
reduce(h, n, A)
{
	var i;

	for (i = n - 1; i > 0; i--) {
		push(A);
		push(stack[h + i]);
		multiply();
		push(stack[h + i - 1]);
		add();
		stack[h + i - 1] = pop();
	}

	if (!iszero(stack[h]))
		stopf("roots: residual error"); // not a root

	// move

	for (i = 0; i < n - 1; i++)
		stack[h + i] = stack[h + i + 1];
}
