#include "defs.h"

void
eval_factor(struct atom *p1)
{
	struct atom *p2;

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
	struct atom *p1, *p2;

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
}

void
factor_term(void)
{
	struct atom *p1;
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
}

void
factor_rational(void)
{
	int h, i, n, t;
	struct atom *p1, *p2;

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

		if (BASE->k != RATIONAL || EXPO->k != RATIONAL) {
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
