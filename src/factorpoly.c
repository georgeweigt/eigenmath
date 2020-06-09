#include "defs.h"

int expo;
struct atom **polycoeff;

#undef POLY
#undef X
#undef Z
#undef A
#undef B
#undef Q
#undef RESULT
#undef YFACTOR

#define POLY p1
#define X p2
#define Z p3
#define A p4
#define B p5
#define Q p6
#define RESULT p7
#define YFACTOR p8

void
factorpoly(void)
{
	save();

	p2 = pop();
	p1 = pop();

	if (!find(p1, p2)) {
		push(p1);
		restore();
		return;
	}

	if (!ispoly(p1, p2)) {
		push(p1);
		restore();
		return;
	}

	if (!issymbol(p2)) {
		push(p1);
		restore();
		return;
	}

	push(p1);
	push(p2);
	factorpoly_nib();

	restore();
}

//	Input:		tos-2		true polynomial
//
//			tos-1		free variable
//
//	Output:		factored polynomial on stack

void
factorpoly_nib(void)
{
	int h, i;

	save();

	X = pop();
	POLY = pop();

	h = tos;

	polycoeff = stack + tos;

	push(POLY);
	push(X);
	expo = coeff() - 1;

	rationalize_coefficients(h);

	// for univariate polynomials we could do expo > 1

	while (expo > 0) {

		if (iszero(polycoeff[0])) {
			push_integer(1);
			A = pop();
			push_integer(0);
			B = pop();
		} else if (get_factor() == 0) {
			break;
		}

		push(A);
		push(X);
		multiply();
		push(B);
		add();
		YFACTOR = pop();

		push(RESULT);
		push(YFACTOR);
		multiply_noexpand();
		RESULT = pop();

		factor_divpoly();

		while (expo && iszero(polycoeff[expo]))
			expo--;
	}

	// unfactored polynomial

	push_integer(0);
	for (i = 0; i <= expo; i++) {
		push(polycoeff[i]);
		push(X);
		push_integer(i);
		power();
		multiply();
		add();
	}
	POLY = pop();

	// factor out negative sign

	if (expo > 0 && isnegativeterm(polycoeff[expo])) {
		push(POLY);
		negate();
		POLY = pop();
		push(RESULT);
		negate_noexpand();
		RESULT = pop();
	}

	push(RESULT);
	push(POLY);
	multiply_noexpand();
	RESULT = pop();

	stack[h] = RESULT;

	tos = h + 1;

	restore();
}

void
rationalize_coefficients(int h)
{
	int i;

	// LCM of all polynomial coefficients

	RESULT = one;
	for (i = h; i < tos; i++) {
		if (isdouble(stack[i])) {
			convert_double_to_rational(stack[i]->u.d);
			stack[i] = pop();
		}
		push(stack[i]);
		denominator();
		push(RESULT);
		lcm();
		RESULT = pop();
	}

	// multiply each coefficient by RESULT

	for (i = h; i < tos; i++) {
		push(RESULT);
		push(stack[i]);
		multiply();
		stack[i] = pop();
	}

	// reciprocate RESULT

	push(RESULT);
	reciprocate();
	RESULT = pop();
}

int
get_factor(void)
{
	int i, j, h;
	int a0, an, na0, nan;

	h = tos;

	an = tos;
	push(polycoeff[expo]);

	divisors_onstack();

	nan = tos - an;

	a0 = tos;
	push(polycoeff[0]);
	divisors_onstack();
	na0 = tos - a0;

	// try roots

	for (i = 0; i < nan; i++) {
		for (j = 0; j < na0; j++) {

			A = stack[an + i];
			B = stack[a0 + j];

			push(B);
			push(A);
			divide();
			negate();
			Z = pop();

			evalpoly();

			if (iszero(Q)) {
				tos = h;
				return 1;
			}

			push(B);
			negate();
			B = pop();

			push(Z);
			negate();
			Z = pop();

			evalpoly();

			if (iszero(Q)) {
				tos = h;
				return 1;
			}
		}
	}

	tos = h;

	return 0;
}

//	Divide a polynomial by Ax+B
//
//	Input:		polycoeff	Dividend coefficients
//
//			expo		Degree of dividend
//
//			A		As above
//
//			B		As above
//
//	Output:		polycoeff	Contains quotient coefficients

void
factor_divpoly(void)
{
	int i;
	Q = zero;
	for (i = expo; i > 0; i--) {
		push(polycoeff[i]);
		polycoeff[i] = Q;
		push(A);
		divide();
		Q = pop();
		push(polycoeff[i - 1]);
		push(Q);
		push(B);
		multiply();
		subtract();
		polycoeff[i - 1] = pop();
	}
	polycoeff[0] = Q;
}

void
evalpoly(void)
{
	int i;
	push_integer(0);
	for (i = expo; i >= 0; i--) {
		push(Z);
		multiply();
		push(polycoeff[i]);
		add();
	}
	Q = pop();
}

void
divisors_onstack(void)
{
	int h, i, k, n;

	save();

	p1 = pop();

	h = tos;

	// push all of the term's factors

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

	restore();
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

#undef ACCUM
#undef BASE
#undef EXPO

#define ACCUM p1
#define BASE p2
#define EXPO p3

void
gen(int h, int k)
{
	int expo, i;

	save();

	ACCUM = pop();

	if (h == k) {
		push(ACCUM);
		restore();
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

	restore();
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
	save();

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
		restore();
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

	restore();
}
