#include "defs.h"

void
factorpoly(void)
{
	int h, i, n;
	struct atom *A, *B, *F, *P, *X;

stop("factorpoly"); // under construction

	X = pop();
	P = pop();

	h = tos;

	push(P);
	push(X);
	push_coeffs();

	rationalize_coeffs(h);

	F = pop();

	while (tos - h > 1) {

		A = pop();

		if (iszero(A))
			continue;

		push(A);

		if (isnegativeterm(A)) {

			for (i = h; i < tos; i++) {
				push(stack[i]);
				negate();
				stack[i] = pop();
			}

			push(F);
			negate_noexpand();
			F = pop();
		}

		if (factorpoly_divisor(h) == 0)
			break;

		B = pop();
		A = pop();

		push(A);
		push(X);
		multiply();
		push(B);
		add();
		push(F);
		multiply_noexpand();
		F = pop();

		factorpoly_divide(h, A, B);

		pop();
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
push_coeffs(void)
{
	int h;
	struct atom *P, *X, *C;

	X = pop();
	P = pop();

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

void
rationalize_coeffs(int h)
{
	int i;
	struct atom *R;

	// R = lcm of coefficients

	R = one;

	for (i = h; i < tos; i++) {
		if (isdouble(stack[i])) {
			convert_double_to_rational(stack[i]->u.d);
			stack[i] = pop();
		}
		push(stack[i]);
		denominator();
		push(R);
		lcm();
		R = pop();
	}

	// multiply coefficients by R

	for (i = h; i < tos; i++) {
		push(R);
		push(stack[i]);
		multiply();
		stack[i] = pop();
	}

	// return 1/R

	push(R);
	reciprocate();
}

int
factorpoly_divisor(int h)
{
	int i, j, h1, h2, n, n1, n2;
	struct atom *A, *B, *Q, *Z;

	n = tos - h;

	h1 = tos;
	push(stack[h + n - 1]);
	push_divisors();
	n1 = tos - h1;

	h2 = tos;
	push(stack[h]);
	push_divisors();
	n2 = tos - h2;

	for (i = 0; i < n1; i++) {
		for (j = 0; j < n2; j++) {

			A = stack[h1 + i];
			B = stack[h2 + j];

			push(B);
			push(A);
			divide();
			negate();
			Z = pop();

			evalpoly(h, n, Z);

			Q = pop();

			if (iszero(Q)) {
				tos = h; // pop all
				push(A);
				push(B);
				return 1;
			}

			push(B);
			negate();
			B = pop();

			push(Z);
			negate();
			Z = pop();

			evalpoly(h, n, Z);

			Q = pop();

			if (iszero(Q)) {
				tos = h1; // pop all
				push(A);
				push(B);
				return 1;
			}
		}
	}

	tos = h1; // pop all

	return 0;
}

// divide by A X + B

void
factorpoly_divide(int h, struct atom *A, struct atom *B)
{
	int i;
	struct atom *Q;

	Q = zero;

	for (i = tos - h - 1; i > 0; i--) {

		push(stack[h + i]);
		stack[h + i] = Q;
		push(A);
		divide();
		Q = pop();

		push(stack[h + i - 1]);
		push(Q);
		push(B);
		multiply();
		subtract();
		stack[h + i - 1] = pop();
	}

	stack[h] = Q;
}

void
evalpoly(int h, int n, struct atom *Q)
{
	int i;
	push_integer(0);
	for (i = n; i > 0; i--) {
		push(Q);
		multiply();
		push(stack[h + i - 1]);
		add();
	}
}

void
push_divisors(void)
{
	int h, i, k, n;
	struct atom *p1, *p2;

	p1 = pop();

	h = tos;

	if (isnum(p1)) {
		push(p1);
		factor_small_number();
	} else if (car(p1) == symbol(ADD)) {
		push(p1);
		factor_add();
	} else if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		if (isnum(car(p1))) {
			push(car(p1));
			factor_small_number();
			p1 = cdr(p1);
		}
		while (iscons(p1)) {
			p2 = car(p1);
			if (car(p2) == symbol(POWER)) {
				push(cadr(p2));
				push(caddr(p2));
			} else {
				push(p2);
				push_integer(1);
			}
			p1 = cdr(p1);
		}
	} else if (car(p1) == symbol(POWER)) {
		push(cadr(p1));
		push(caddr(p1));
	} else {
		push(p1);
		push_integer(1);
	}

	k = tos;

	// contruct divisors by recursive descent

	push_integer(1);

	gen(h, k);

	// move

	n = tos - k;

	for (i = 0; i < n; i++)
		stack[h + i] = stack[k + i];

	tos = h + n;
}

//	Generate divisors
//
//	Input:		Base-exponent pairs on stack
//
//			h	first pair
//
//			k	just past last pair
//
//	Output:		Divisors on stack
//
//	For example, factor list 2 2 3 1 results in 6 divisors,
//
//		1
//		3
//		2
//		6
//		4
//		12

void
gen(int h, int k)
{
	int expo, i;
	struct atom *ACCUM, *BASE, *EXPO;

	ACCUM = pop();

	if (h == k) {
		push(ACCUM);
		return;
	}

	BASE = stack[h + 0];
	EXPO = stack[h + 1];

	push(EXPO);
	expo = pop_integer();

	for (i = 0; i <= abs(expo); i++) {
		push(ACCUM);
		push(BASE);
		push_integer(sign(expo) * i);
		power();
		multiply();
		gen(h + 2, k);
	}
}

//	Factor ADD expression
//
//	Input:		Expression on stack
//
//	Output:		Factors on stack
//
//	Each factor consists of two expressions, the factor itself followed
//	by the exponent.

void
factor_add(void)
{
	struct atom *p1, *p2, *p3;

	p1 = pop();

	// get gcd of all terms

	p3 = cdr(p1);
	push(car(p3));
	p3 = cdr(p3);
	while (iscons(p3)) {
		push(car(p3));
		gcd();
		p3 = cdr(p3);
	}

	// check gcd

	p2 = pop();
	if (isplusone(p2)) {
		push(p1);
		push_integer(1);
		return;
	}

	// push factored gcd

	if (isnum(p2)) {
		push(p2);
		factor_small_number();
	} else if (car(p2) == symbol(MULTIPLY)) {
		p3 = cdr(p2);
		if (isnum(car(p3))) {
			push(car(p3));
			factor_small_number();
		} else {
			push(car(p3));
			push_integer(1);
		}
		p3 = cdr(p3);
		while (iscons(p3)) {
			push(car(p3));
			push_integer(1);
			p3 = cdr(p3);
		}
	} else {
		push(p2);
		push_integer(1);
	}

	// divide each term by gcd

	push(p2);
	reciprocate();
	p2 = pop();

	push_integer(0);
	p3 = cdr(p1);
	while (iscons(p3)) {
		push(p2);
		push(car(p3));
		multiply();
		add();
		p3 = cdr(p3);
	}

	push_integer(1);
}
