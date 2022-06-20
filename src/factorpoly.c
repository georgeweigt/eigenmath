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
	int i, j, h1, h2, n, n1, n2;
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
	push_divisors();
	n1 = tos - h1;

	h2 = tos;
	push(C);
	denominator();
	push_divisors();
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
push_divisors(void)
{
	int h, i, k, n;
	struct atom *p1;

	p1 = pop();

	h = tos;

	push(p1);
	factor_small_number();

#if 0
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
#endif

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
