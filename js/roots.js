function
roots()
{
	var h, i, j, n;
	var A, C, LIST, P, X;

	X = pop();
	P = pop();

	h = stack.length;

	coeffs(P, X); // put coeffs on stack

	// check coeffs

	for (i = h; i < stack.length; i++)
		if (!isrational(stack[i]))
			stopf("roots: coeffs");

	LIST = symbol(NIL);

	while (stack.length - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue; // coeff of monomial is zero

		// divide through by C

		for (i = h; i < stack.length; i++) {
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

	stack.splice(h); // pop all

	n = lengthf(LIST);

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
		stack.splice(h + 1);
		return;
	}

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A.elem[i] = stack[h + i];

	stack.splice(h); // pop all

	push(A);
}

// coefficients are on the stack

function
findroot(h)
{
	var i, j, k, m, n;
	var A, C, T;

	C = stack[h]; // constant term

	if (iszero(C)) {
		push_integer(0); // root is zero
		return 1;
	}

	k = stack.length;
	push(C);
	numerator();
	n = pop_integer();
	divisors(n); // push divisors of n

	m = stack.length;
	push(C);
	denominator();
	n = pop_integer();
	divisors(n); // push divisors of n

	for (i = k; i < m; i++) {
		for (j = m; j < stack.length; j++) {

			// try positive A

			push(stack[i]);
			push(stack[j]);
			divide();
			A = pop();

			horner(h, k, A);

			T = pop();

			if (iszero(T)) {
				stack.splice(k); // pop all
				push(A);
				return 1; // root on stack
			}

			// try negative A

			push(A);
			negate();
			A = pop();

			horner(h, k, A);

			T = pop();

			if (iszero(T)) {
				stack.splice(k); // pop all
				push(A);
				return 1; // root on stack
			}
		}
	}

	stack.splice(k); // pop all

	return 0; // no root
}

// evaluate p(x) at x = A using horner's rule

function
horner(h, k, A)
{
	var i;

	push(stack[k - 1]);

	for (i = k - 2; i >= h; i--) {
		push(A);
		multiply();
		push(stack[i]);
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
reduce(h, A)
{
	var i, t;

	t = stack.length - 1;

	for (i = t; i > h; i--) {
		push(A);
		push(stack[i]);
		multiply();
		push(stack[i - 1]);
		add();
		stack[i - 1] = pop();
	}

	if (!iszero(stack[h]))
		stopf("roots: residual error"); // not a root

	for (i = h; i < t; i++)
		stack[i] = stack[i + 1];

	pop(); // one less coeff on stack
}
