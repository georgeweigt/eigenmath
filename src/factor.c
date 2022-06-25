#include "defs.h"

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

	factorpoly_coeffs(P, X); // put coeffs on stack

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

// pushes coeffs on stack

void
factorpoly_coeffs(struct atom *P, struct atom *X)
{
	struct atom *C;

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

// factors N or N^M where N and M are rational numbers, returns factors on stack

void
factor_factor(void)
{
	uint32_t *numer, *denom;
	struct atom *FARG, *BASE, *EXPO;

	FARG = pop();

	if (car(FARG) == symbol(POWER)) {

		BASE = cadr(FARG);
		EXPO = caddr(FARG);

		if (!isrational(BASE) || !isrational(EXPO)) {
			push(FARG);
			return;
		}

		if (isminusone(BASE)) {
			push(FARG); // -1 to the M
			return;
		}

		if (BASE->sign == MMINUS) {
			push_symbol(POWER);
			push_integer(-1);
			push(EXPO);
			list(3); // leave on stack
		}

		numer = BASE->u.q.a;
		denom = BASE->u.q.b;

		if (!MEQUAL(numer, 1))
			factor_bignum(numer, EXPO);

		if (!MEQUAL(denom, 1)) {
			// flip sign of exponent
			push(EXPO);
			negate();
			EXPO = pop();
			factor_bignum(denom, EXPO);
		}

		return;
	}

	if (!isrational(FARG) || iszero(FARG) || isplusone(FARG) || isminusone(FARG)) {
		push(FARG);
		return;
	}

	if (FARG->sign == MMINUS)
		push_integer(-1);

	numer = FARG->u.q.a;
	denom = FARG->u.q.b;

	if (!MEQUAL(numer, 1)) {
		EXPO = one;
		factor_bignum(numer, EXPO);
	}

	if (!MEQUAL(denom, 1)) {
		EXPO = minusone;
		factor_bignum(denom, EXPO);
	}
}

void
factor_bignum(uint32_t *a, struct atom *EXPO)
{
	int k, m;
	uint32_t d, n;
	struct atom *P;

	if (MLENGTH(a) > 1) {
		// too big to factor
		push_bignum(MPLUS, mcopy(a), mint(1));
		if (!isplusone(EXPO)) {
			push_symbol(POWER);
			swap();
			push(EXPO);
			list(3);
		}
		return;
	}

	n = a[0];

	for (k = 0; k < MAXPRIMETAB; k++) {

		d = primetab[k];

		if (n / d < d)
			break; // n is 1 or prime

		m = 0;

		while (n % d == 0) {
			n /= d;
			m++;
		}

		if (m == 0)
			continue;

		push_bignum(MPLUS, mint(d), mint(1));

		push_integer(m);
		push(EXPO);
		multiply();
		P = pop();

		if (!isplusone(P)) {
			push_symbol(POWER);
			swap();
			push(P);
			list(3);
		}
	}

	if (n > 1) {
		push_bignum(MPLUS, mint(n), mint(1));
		if (!isplusone(EXPO)) {
			push_symbol(POWER);
			swap();
			push(EXPO);
			list(3);
		}
	}
}
