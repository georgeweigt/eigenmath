#include "defs.h"

void
eval_factor(void)
{
	push(cadr(p1));
	eval();

	p2 = pop();

	if (isrational(p2)) {
		push(p2);
		factor_rational();
		return;
	}

	if (isdouble(p2)) {
		convert_double_to_rational(p2->u.d);
		factor_rational();
		return;
	}

	push(p2);

	push(caddr(p1));
	eval();

	p2 = pop();

	if (p2 == symbol(NIL))
		push_symbol(X_LOWER);
	else
		push(p2);

	factorpoly();

	// more factoring?

	p1 = cdddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		factor_again();
		p1 = cdr(p1);
	}
}

void
factor_again(void)
{
	int h, n;

	save();

	p2 = pop();
	p1 = pop();

	h = tos;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			push(p2);
			factor_term();
			p1 = cdr(p1);
		}
	} else {
		push(p1);
		push(p2);
		factor_term();
	}

	n = tos - h;

	if (n > 1)
		multiply_factors_noexpand(n);

	restore();
}

void
factor_term(void)
{
	save();
	factorpoly();
	p1 = pop();
	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			p1 = cdr(p1);
		}
	} else
		push(p1);
	restore();
}

void
factor_rational(void)
{
	int h, i, n, t;

	save();

	p1 = pop();

	h = tos;

	// factor numerator

	push(p1);
	numerator();
	p2 = pop();
	if (!isplusone(p2)) {
		push(p2);
		factor_number();
	}

	// factor denominator

	push(p1);
	denominator();
	p2 = pop();
	if (!isplusone(p2)) {
		t = tos;
		push(p2);
		factor_number();
		for (i = t; i < tos; i++) {
			p2 = stack[i];
			if (car(p2) == symbol(POWER)) {
				push_symbol(POWER);
				push(cadr(p2)); // base
				push(caddr(p2)); // exponent
				negate();
				list(3);
			} else {
				push_symbol(POWER);
				push(p2); // base
				push_integer(-1); // exponent
				list(3);
			}
			stack[i] = pop();
		}
	}

	n = tos - h;

	if (n == 0)
		push(one);
	else if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // make MULTIPLY head of list
	}

	restore();
}

// for factoring small integers (2^32 or less)

void
factor_small_number(void)
{
	int d, k, m, n;

	save();

	n = pop_integer();

	if (n == ERR)
		stop("number too big to factor");

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
			push_integer(d);
			push_integer(m);
		}
	}

	if (n > 1) {
		push_integer(n);
		push_integer(1);
	}

	restore();
}

#undef FARG
#undef BASE
#undef EXPO
#undef P

#define FARG p1
#define BASE p2
#define EXPO p3
#define P p4

// factors N or N^M where N and M are rational numbers, returns factors on stack

void
factor_factor(void)
{
	save();
	factor_factor_nib();
	restore();
}

void
factor_factor_nib(void)
{
	uint32_t *numer, *denom;

	FARG = pop();

	if (car(FARG) == symbol(POWER)) {

		BASE = cadr(FARG);
		EXPO = caddr(FARG);

		if (BASE->k != RATIONAL || EXPO->k != RATIONAL) {
			push(FARG);
			return;
		}

		if (equaln(BASE, -1)) {
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
			factor_bignum(numer);

		if (!MEQUAL(denom, 1)) {
			// flip sign of exponent
			push(EXPO);
			negate();
			EXPO = pop();
			factor_bignum(denom);
		}

		return;
	}

	if (FARG->k != RATIONAL || iszero(FARG) || isplusone(FARG) || isminusone(FARG)) {
		push(FARG);
		return;
	}

	if (FARG->sign == MMINUS)
		push_integer(-1);

	numer = FARG->u.q.a;
	denom = FARG->u.q.b;

	if (!MEQUAL(numer, 1)) {
		EXPO = one;
		factor_bignum(numer);
	}

	if (!MEQUAL(denom, 1)) {
		EXPO = minusone;
		factor_bignum(denom);
	}
}

void
factor_bignum(uint32_t *a)
{
	int d, k, m;
	uint32_t n;

	if (MLENGTH(a) > 1) {
		// too big to factor
		push_rational_number(MPLUS, mcopy(a), mint(1));
		if (!equaln(EXPO, 1)) {
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

		push_rational_number(MPLUS, mint(d), mint(1));

		push_integer(m);
		push(EXPO);
		multiply();
		P = pop();

		if (!equaln(P, 1)) {
			push_symbol(POWER);
			swap();
			push(P);
			list(3);
		}
	}

	if (n > 1) {
		push_rational_number(MPLUS, mint(n), mint(1));
		if (!equaln(EXPO, 1)) {
			push_symbol(POWER);
			swap();
			push(EXPO);
			list(3);
		}
	}
}
