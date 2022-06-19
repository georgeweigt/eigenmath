#include "defs.h"

// BASE and EXPO are numbers

void
power_numbers(struct atom *BASE, struct atom *EXPO)
{
	int h, i, j, n;
	uint32_t *a, *b;
	struct atom *p1, *p2;

	// n^0

	if (iszero(EXPO)) {
		push_integer(1);
		return;
	}

	// 0^n

	if (iszero(BASE)) {
		if (isnegativenumber(EXPO))
			stop("divide by zero");
		push_integer(0);
		return;
	}

	// 1^n

	if (isplusone(BASE)) {
		push_integer(1);
		return;
	}

	// n^1

	if (isplusone(EXPO)) {
		push(BASE);
		return;
	}

	if (isdouble(BASE) || isdouble(EXPO)) {
		power_double(BASE, EXPO);
		return;
	}

	// integer exponent?

	if (isinteger(EXPO)) {
		a = mpow(BASE->u.q.a, EXPO->u.q.a);
		b = mpow(BASE->u.q.b, EXPO->u.q.a);
		if (isnegativenumber(BASE) && (EXPO->u.q.a[0] & 1))
			if (isnegativenumber(EXPO))
				push_bignum(MMINUS, b, a); // reciprocate
			else
				push_bignum(MMINUS, a, b);
		else
			if (isnegativenumber(EXPO))
				push_bignum(MPLUS, b, a); // reciprocate
			else
				push_bignum(MPLUS, a, b);
		return;
	}

	// exponent is a root

	h = tos;

	// put factors on stack

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);

	factor_factor();

	// normalize factors

	n = tos - h; // fix n now, stack can grow

	for (i = 0; i < n; i++) {
		p1 = stack[h + i];
		if (car(p1) == symbol(POWER)) {
			BASE = cadr(p1);
			EXPO = caddr(p1);
			power_numbers_factor(BASE, EXPO);
			stack[h + i] = pop(); // fill hole
		}
	}

	// combine numbers (leaves radicals on stack)

	p1 = one;

	for (i = h; i < tos; i++) {
		p2 = stack[i];
		if (isnum(p2)) {
			push(p1);
			push(p2);
			multiply();
			p1 = pop();
			for (j = i + 1; j < tos; j++)
				stack[j - 1] = stack[j];
			tos--;
			i--;
		}
	}

	// finalize

	n = tos - h;

	if (n == 0 || !isplusone(p1)) {
		push(p1);
		n++;
	}

	if (n == 1)
		return;

	sort_factors(n);
	list(n);
	push_symbol(MULTIPLY);
	swap();
	cons();
}

// BASE is an integer

void
power_numbers_factor(struct atom *BASE, struct atom *EXPO)
{
	uint32_t *a, *b, *n, *q, *r;
	struct atom *p0;

	if (isminusone(BASE)) {
		power_minusone(EXPO);
		p0 = pop();
		if (car(p0) == symbol(MULTIPLY)) {
			p0 = cdr(p0);
			while (iscons(p0)) {
				push(car(p0));
				p0 = cdr(p0);
			}
		} else
			push(p0);
		return;
	}

	if (isinteger(EXPO)) {
		a = mpow(BASE->u.q.a, EXPO->u.q.a);
		b = mint(1);
		if (isnegativenumber(EXPO))
			push_bignum(MPLUS, b, a); // reciprocate
		else
			push_bignum(MPLUS, a, b);
		return;
	}

	// EXPO.a          r
	// ------ == q + ------
	// EXPO.b        EXPO.b

	q = mdiv(EXPO->u.q.a, EXPO->u.q.b);
	r = mmod(EXPO->u.q.a, EXPO->u.q.b);

	// process q

	if (!MZERO(q)) {
		a = mpow(BASE->u.q.a, q);
		b = mint(1);
		if (isnegativenumber(EXPO))
			push_bignum(MPLUS, b, a); // reciprocate
		else
			push_bignum(MPLUS, a, b);
	}

	mfree(q);

	// process r

	if (MLENGTH(BASE->u.q.a) == 1) {
		// BASE is 32 bits or less, hence BASE is a prime number, no root
		push_symbol(POWER);
		push(BASE);
		push_bignum(EXPO->sign, r, mcopy(EXPO->u.q.b)); // r used here, r is not leaked
		list(3);
		return;
	}

	// BASE was too big to factor, try finding root

	n = mroot(BASE->u.q.a, EXPO->u.q.b);

	if (n == NULL) {
		// no root
		push_symbol(POWER);
		push(BASE);
		push_bignum(EXPO->sign, r, mcopy(EXPO->u.q.b)); // r used here, r is not leaked
		list(3);
		return;
	}

	// raise n to rth power

	a = mpow(n, r);
	b = mint(1);

	mfree(n);
	mfree(r);

	if (isnegativenumber(EXPO))
		push_bignum(MPLUS, b, a); // reciprocate
	else
		push_bignum(MPLUS, a, b);
}

void
power_double(struct atom *BASE, struct atom *EXPO)
{
	double base, d, expo;

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	if (base > 0.0 || expo == floor(expo)) {
		d = pow(base, expo);
		push_double(d);
		return;
	}

	// BASE is negative and EXPO is fractional

	power_minusone(EXPO);

	if (base == -1.0)
		return;

	d = pow(-base, expo);
	push_double(d);

	multiply();
}
