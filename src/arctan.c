#include "defs.h"

void
eval_arctan(void)
{
	push(cadr(p1));
	eval();
	if (caddr(p1) == symbol(NIL))
		push_integer(1);
	else {
		push(caddr(p1));
		eval();
	}
	arctan();
}

#undef T
#undef X
#undef Y
#undef Z

#define T p1
#define X p2
#define Y p3
#define Z p4

void
arctan(void)
{
	save();
	arctan_nib();
	restore();
}

void
arctan_nib(void)
{
	X = pop();
	Y = pop();

	if (isnum(X) && isnum(Y)) {
		arctan_numerical_args();
		return;
	}

	// arctan(z) = 1/2 i log((i + z) / (i - z))

	if (!iszero(X) && (isdoublez(X) || isdoublez(Y))) {
		push(Y);
		push(X);
		divide();
		Z = pop();
		push_double(0.5);
		push(imaginaryunit);
		multiply();
		push(imaginaryunit);
		push(Z);
		add();
		push(imaginaryunit);
		push(Z);
		subtract();
		divide();
		logarithm();
		multiply();
		return;
	}

	if (car(Y) == symbol(TAN) && isplusone(X)) {
		push(cadr(Y)); // x of tan(x)
		return;
	}

	push_symbol(ARCTAN);
	push(Y);
	push(X);
	list(3);
}

void
arctan_numerical_args(void)
{
	double x, y;

	if (iszero(X) && iszero(Y)) {
		push_symbol(ARCTAN);
		push_integer(0);
		push_integer(0);
		list(3);
		return;
	}

	if (isnum(X) && isnum(Y) && (isdouble(X) || isdouble(Y))) {
		push(X);
		x = pop_double();
		push(Y);
		y = pop_double();
		push_double(atan2(y, x));
		return;
	}

	// X and Y are rational numbers

	if (iszero(Y)) {
		if (X->sign == MPLUS)
			push_integer(0);
		else
			push_integer(-1);
		push_symbol(PI);
		multiply();
		return;
	}

	if (iszero(X)) {
		if (Y->sign == MPLUS)
			push_rational(1, 2);
		else
			push_rational(-1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	// convert fractions to integers

	push(Y);
	push(X);
	divide();
	absv();
	T = pop();

	push(T);
	numerator();
	if (Y->sign == MMINUS)
		negate();
	Y = pop();

	push(T);
	denominator();
	if (X->sign == MMINUS)
		negate();
	X = pop();

	// compare numerators and denominators, ignore signs

	if (mcmp(X->u.q.a, Y->u.q.a) || mcmp(X->u.q.b, Y->u.q.b)) {
		// not equal
		push_symbol(ARCTAN);
		push(Y);
		push(X);
		list(3);
		return;
	}

	// X = Y modulo sign

	if (X->sign == MPLUS)
		if (Y->sign == MPLUS)
			push_rational(1, 4);
		else
			push_rational(-1, 4);
	else
		if (Y->sign == MPLUS)
			push_rational(3, 4);
		else
			push_rational(-3, 4);

	push_symbol(PI);
	multiply();
}
