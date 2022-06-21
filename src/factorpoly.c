#include "defs.h"

void
factorpoly(void)
{
	int h, i, n;
	struct atom *C, *F, *P, *R, *X;

	X = pop();
	P = pop();

	h = tos;

	factorpoly_coeffs(P, X); // put coeffs on stack

	F = one;

	while (tos - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue;

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

		R = pop();

		push(F);
		push(X);
		push(R);
		subtract();
		multiply_noexpand();
		F = pop();

		factorpoly_divide(h, R);

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

void
factorpoly_coeffs(struct atom *P, struct atom *X)
{
	int h;
	struct atom *C;

	h = tos;

	for (;;) {

		push(P);
		push(X);
		push_integer(0);
		subst();
		eval();
		C = pop();

		push(C);

		push(P);
		push(C);
		subtract();
		P = pop();

		if (iszero(P))
			break;

		push(P);
		push(X);
		divide();
		P = pop();
	}
}

int
factorpoly_root(int h)
{
	int a, i, j, h1, h2, n, n1, n2;
	struct atom *C, *T, *X;

	n = tos - h;

	C = stack[h]; // constant term

	if (!isrational(C))
		stop("factor");

	if (iszero(C)) {
		push(C);
		return 1;
	}

	h1 = tos;
	push(C);
	numerator();
	a = pop_integer();
	factorpoly_push_divisors(a);
	n1 = tos - h1;

	h2 = tos;
	push(C);
	denominator();
	a = pop_integer();
	factorpoly_push_divisors(a);
	n2 = tos - h2;

	for (i = 0; i < n1; i++) {
		for (j = 0; j < n2; j++) {

			push(stack[h1 + i]);
			push(stack[h2 + j]);
			divide();
			X = pop();

			factorpoly_eval(h, n, X);

			T = pop();

			if (iszero(T)) {
				tos = h + n; // pop all
				push(X);
				return 1;
			}

			push(X);
			negate();
			X = pop();

			factorpoly_eval(h, n, X);

			T = pop();

			if (iszero(T)) {
				tos = h + n; // pop all
				push(X);
				return 1;
			}
		}
	}

	tos = h + n; // pop all

	return 0; // no root
}

// divide by X - R where R is a root

void
factorpoly_divide(int h, struct atom *R)
{
	int i;
	struct atom *C;

	C = one;

	for (i = tos - 2; i > h; i--) {

		push(stack[i]);
		push(C);
		push(R);
		multiply();
		add();

		stack[i] = C;

		C = pop();
	}

	stack[h] = C;
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

void
factorpoly_push_divisors(int a)
{
	int h, i, k, n;

	h = tos;

	factorpoly_factor_small_number(a);

	k = tos;

	// contruct divisors by recursive descent

	push_integer(1);

	factorpoly_gen(h, k);

	// move

	n = tos - k;

	for (i = 0; i < n; i++)
		stack[h + i] = stack[k + i];

	tos = h + n;
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
factorpoly_gen(int h, int k)
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
		factorpoly_gen(h + 2, k);
	}
}

// n is 32 bits or less

void
factorpoly_factor_small_number(int n)
{
	int d, k, m;

	if (n < 0)
		n = -n;

	for (k = 0; k < MAXPRIMETAB; k++) {

		d = primetab[k];

		if (n / d < d)
			break; // n is 1 or prime

		m = 0;

		while (n % d == 0) {
			n /= d;
			m++;
		}

		if (m) {
			push_integer(d); // push pair
			push_integer(m);
		}
	}

	if (n > 1) {
		push_integer(n); // push pair
		push_integer(1);
	}
}
