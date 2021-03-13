#include "defs.h"

#undef BASE
#undef EXPO

#define BASE p1
#define EXPO p2

void
eval_power(void)
{
	int t;
	push(caddr(p1)); // exponent
	eval();
	p2 = pop();
	if (isnegativenumber(p2)) {
		// don't expand in denominators
		t = expanding;
		expanding = 0;
		push(cadr(p1)); // base
		eval();
		push(p2);
		power();
		expanding = t;
	} else {
		push(cadr(p1)); // base
		eval();
		push(p2);
		power();
	}
}

void
power(void)
{
	save();
	power_nib();
	restore();
}

void
power_nib(void)
{
	int h;

	EXPO = pop();
	BASE = pop();

	if (istensor(BASE)) {
		power_tensor();
		return;
	}

	if (iszero(BASE) && isnegativenumber(EXPO))
		stop("divide by zero");

	if (BASE == symbol(EXP1) && isdouble(EXPO)) {
		push_double(M_E);
		BASE = pop();
	}

	if (BASE == symbol(PI) && isdouble(EXPO)) {
		push_double(M_PI);
		BASE = pop();
	}

	if (power_precheck())
		return;

	// BASE and EXPO numerical?

	if (isnum(BASE) && isnum(EXPO)) {
		power_numbers();
		return;
	}

	// BASE = e ?

	if (BASE == symbol(EXP1)) {
		power_natural_number();
		return;
	}

	// do this before checking for (a + b)^n

	if (iscomplexnumber(BASE)) {
		power_complex_number();
		return;
	}

	// (a + b)^n -> (a + b) * (a + b) ...

	if (car(BASE) == symbol(ADD)) {
		power_sum();
		return;
	}

	// (a * b) ^ c -> (a ^ c) * (b ^ c)

	if (car(BASE) == symbol(MULTIPLY)) {
		h = tos;
		p3 = cdr(BASE);
		while (iscons(p3)) {
			push(car(p3));
			push(EXPO);
			power();
			p3 = cdr(p3);
		}
		multiply_factors(tos - h);
		return;
	}

	// (a^b)^c -> a^(b * c)

	if (car(BASE) == symbol(POWER)) {
		push(cadr(BASE));
		push(caddr(BASE));
		push(EXPO);
		multiply_expand(); // always expand products of exponents
		power();
		return;
	}

	// none of the above

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);
}

int
power_precheck(void)
{
	// 1^expr = expr^0 = 1

	if (equaln(BASE, 1) || equaln(EXPO, 0)) {
		if (isdouble(BASE) || isdouble(EXPO))
			push_double(1.0);
		else
			push_integer(1);
		return 1;
	}

	// expr^1 = expr

	if (equaln(EXPO, 1)) {
		push(BASE);
		return 1;
	}

	// 0^expr

	if (equaln(BASE, 0)) {
		if (isnum(EXPO)) {
			if (isdouble(BASE) || isdouble(EXPO))
				push_double(0.0);
			else
				push_integer(0);
		} else {
			push_symbol(POWER);
			push(BASE);
			push(EXPO);
			list(3);
		}
		return 1;
	}

	return 0;
}

// BASE = e

void
power_natural_number(void)
{
	double x, y;

	// exp(x + i y) = exp(x) (cos(y) + i sin(y))

	if (isdoublez(EXPO)) {
		if (car(EXPO) == symbol(ADD)) {
			x = cadr(EXPO)->u.d;
			y = cadaddr(EXPO)->u.d;
		} else {
			x = 0.0;
			y = cadr(EXPO)->u.d;
		}
		push_double(exp(x));
		push_double(y);
		scos();
		push(imaginaryunit);
		push_double(y);
		ssin();
		multiply();
		add();
		multiply();
		return;
	}

	// e^log(expr) -> expr

	if (car(EXPO) == symbol(LOG)) {
		push(cadr(EXPO));
		return;
	}

	if (simplify_polar_expr())
		return;

	// none of the above

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);
}

int
simplify_polar_expr(void)
{
	if (car(EXPO) == symbol(ADD)) {
		p3 = cdr(EXPO);
		while (iscons(p3)) {
			if (simplify_polar_term(car(p3))) {
				push(EXPO);
				push(car(p3));
				subtract();
				exponential();
				multiply();
				return 1;
			}
			p3 = cdr(p3);
		}
		return 0;
	}

	return simplify_polar_term(EXPO);
}

int
simplify_polar_term(struct atom *p)
{
	int n;
	double d;

	if (car(p) != symbol(MULTIPLY))
		return 0;

	// exp(i pi) -> -1

	if (length(p) == 3 && isimaginaryunit(cadr(p)) && caddr(p) == symbol(PI)) {
		push_integer(-1);
		return 1;
	}

	if (length(p) != 4 || !isnum(cadr(p)) || !isimaginaryunit(caddr(p)) || cadddr(p) != symbol(PI))
		return 0;

	p = cadr(p); // coeff

	if (isdouble(p)) {
		d = p->u.d;
		if (floor(d) == d) {
			if (fmod(d, 2.0) == 0.0)
				push_double(1.0);
			else
				push_double(-1.0);
			return 1;
		}
		if (floor(d) + 0.5 == d) {
			n = (int) (d / 0.5) % 4;
			if (n == 1 || n == -3) {
				push_symbol(MULTIPLY);
				push_double(1.0);
				push(imaginaryunit);
				list(3);
			} else {
				push_symbol(MULTIPLY);
				push_double(-1.0);
				push(imaginaryunit);
				list(3);
			}
			return 1;
		}
		return 0;
	}

	if (MEQUAL(p->u.q.b, 1)) {
		if (p->u.q.a[0] % 2 == 0)
			push_integer(1);
		else
			push_integer(-1);
		return 1;
	}

	if (MEQUAL(p->u.q.b, 2)) {
		n = p->u.q.a[0] % 4;
		if ((n == 1 && p->sign == MPLUS) || (n == 3 && p->sign == MMINUS))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		}
		return 1;
	}

	return 0;
}

// (a + b)^n -> (a + b) * (a + b) ...

void
power_sum(void)
{
	int h, i, m, n;

	if (expanding == 0 || !isnum(EXPO) || isnegativenumber(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	push(EXPO);

	n = pop_integer();

	if (n == ERR) {
		// expo exceeds int max
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	// square the sum first (prevents infinite loop through multiply)

	h = tos;

	p3 = cdr(BASE);

	while (iscons(p3)) {
		p4 = cdr(BASE);
		while (iscons(p4)) {
			push(car(p3));
			push(car(p4));
			multiply();
			p4 = cdr(p4);
		}
		p3 = cdr(p3);
	}

	add_terms(tos - h);

	// continue up to power m

	m = abs(n);

	for (i = 2; i < m; i++) {
		push(BASE);
		multiply();
	}

	if (n < 0) {
		push_symbol(POWER);
		swap();
		push_integer(-1);
		list(3);
	}
}

// convert (-1)^(a/b) to c*(-1)^(x) where c=1 or c=-1 and -1/2 < (x) <= 1/2
//
// r = a mod b (remainder of a/b)
//
// q = (a/b - r) mod 2
//
// s = sign of a/b
//
// case			c	x
//
// s=1 q=0 r/b <= 1/2	1	r/b
// s=1 q=0 r/b > 1/2	-1	-(b - r)/b
//
// s=1 q=1 r/b <= 1/2	-1	r/b
// s=1 q=1 r/b > 1/2	1	-(b - r)/b
//
// s=-1 q=0 r/b < 1/2	1	-r/b
// s=-1 q=0 r/b >= 1/2	-1	(b - r)/b
//
// s=-1 q=1 r/b < 1/2	-1	-r/b
// s=-1 q=1 r/b >= 1/2	1	(b - r)/b
//
// q=1 flips sign of c
//
// s=-1 flips sign of x

void
power_imaginary_unit(void)
{
	int c, s;
	uint32_t *a, *b, *q, *r, *t;
	double theta, x, y;

	if (!isnum(EXPO)) {
		push_symbol(POWER);
		push_integer(-1);
		push(EXPO);
		list(3);
		if (isdouble(BASE)) {
			// BASE = -1.0, keep double
			push_symbol(MULTIPLY);
			swap();
			push_double(1.0);
			swap();
			list(3);
		}
		return;
	}

	if (equalq(EXPO, 1, 2)) {
		push(imaginaryunit);
		if (isdouble(BASE)) {
			// BASE = -1.0, keep double
			push_symbol(MULTIPLY);
			swap();
			push_double(1.0);
			swap();
			list(3);
		}
		return;
	}

	if (isdouble(EXPO)) {
		theta = EXPO->u.d * M_PI; // pi is the polar angle for -1
		x = cos(theta);
		y = sin(theta);
		if (fabs(x) < 1e-12)
			x = 0.0;
		if (fabs(y) < 1e-12)
			y = 0.0;
		push_double(x);
		push_double(y);
		push(imaginaryunit);
		multiply();
		add();
		return;
	}

	// integer exponent?

	if (isinteger(EXPO)) {
		if (EXPO->u.q.a[0] % 2 == 1) {
			// odd exponent
			if (isdouble(BASE))
				push_double(-1.0); // BASE = -1.0, keep double
			else
				push_integer(-1);
		} else {
			// even exponent
			if (isdouble(BASE))
				push_double(1.0); // BASE = -1.0, keep double
			else
				push_integer(1);
		}
		return;
	}

	a = EXPO->u.q.a;
	b = EXPO->u.q.b;

	q = mdiv(a, b);
	r = mmod(a, b);

	t = madd(r, r);

	switch (mcmp(t, b)) {
	case -1:
		a = mcopy(r);
		c = 1;
		s = 1;
		break;
	case 0:
		if (EXPO->sign == MPLUS) {
			a = mcopy(r);
			c = 1;
			s = 1;
		} else {
			a = msub(b, r);
			c = -1;
			s = -1;
		}
		break;
	case 1:
		a = msub(b, r);
		c = -1;
		s = -1;
		break;
	}

	if (q[0] % 2 == 1)
		c *= -1;

	if (EXPO->sign == MMINUS)
		s *= -1;

	b = mcopy(b);

	push_symbol(POWER);
	push_integer(-1);
	if (s == 1)
		push_rational_number(MPLUS, a, b);
	else
		push_rational_number(MMINUS, a, b);
	list(3);

	if (c == -1) {
		push_symbol(MULTIPLY);
		swap();
		if (isdouble(BASE))
			push_double(-1.0); // BASE = -1.0, keep double
		else
			push_integer(-1);
		swap();
		list(3);
	} else if (isdouble(BASE)) {
		// BASE = -1.0, keep double
		push_symbol(MULTIPLY);
		swap();
		push_double(1.0);
		swap();
		list(3);
	}

	mfree(q);
	mfree(r);
	mfree(t);
}

#undef R
#undef X
#undef Y
#undef PX
#undef PY

#define R p3
#define X p4
#define Y p5
#define PX p6
#define PY p7

// BASE is rectangular complex numerical

void
power_complex_number(void)
{
	int n;

	if (!isnum(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	// lisp(2+3*i) = (add 2 (multiply 3 (power -1 1/2)))

	// lisp(1+i) = (add 1 (power -1 1/2))

	// lisp(3*i) = (multiply 3 (power -1 1/2))

	// lisp(i) = (power -1 1/2)

	if (car(BASE) == symbol(ADD)) {
		X = cadr(BASE);
		if (caaddr(BASE) == symbol(MULTIPLY))
			Y = cadaddr(BASE);
		else
			Y = one;
	} else if (car(BASE) == symbol(MULTIPLY)) {
		X = zero;
		Y = cadr(BASE);
	} else {
		X = zero;
		Y = one;
	}

	if (isdouble(X) || isdouble(Y) || isdouble(EXPO)) {
		power_complex_double();
		return;
	}

	if (!isinteger(EXPO)) {
		power_complex_rational();
		return;
	}

	push(EXPO);
	n = pop_integer();

	if (n == ERR) {
		// exponent exceeds int max
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (n > 0)
		power_complex_plus(n);
	else if (n < 0)
		power_complex_minus(-n);
	else
		push_integer(1);
}

void
power_complex_plus(int n)
{
	int i;

	PX = X;
	PY = Y;

	for (i = 1; i < n; i++) {

		push(PX);
		push(X);
		multiply();
		push(PY);
		push(Y);
		multiply();
		subtract();

		push(PX);
		push(Y);
		multiply();
		push(PY);
		push(X);
		multiply();
		add();

		PY = pop();
		PX = pop();
	}

	// X + i*Y

	push(PX);
	push(imaginaryunit);
	push(PY);
	multiply();
	add();
}

//
//               /        \  n
//         -n   |  X - iY  |
// (X + iY)   = | -------- |
//              |   2   2  |
//               \ X + Y  /

// X and Y are rational numbers

void
power_complex_minus(int n)
{
	int i;

	// R = X^2 + Y^2

	push(X);
	push(X);
	multiply();
	push(Y);
	push(Y);
	multiply();
	add();
	R = pop();

	// X = X / R

	push(X);
	push(R);
	divide();
	X = pop();

	// Y = -Y / R

	push(Y);
	negate();
	push(R);
	divide();
	Y = pop();

	PX = X;
	PY = Y;

	for (i = 1; i < n; i++) {

		push(PX);
		push(X);
		multiply();
		push(PY);
		push(Y);
		multiply();
		subtract();

		push(PX);
		push(Y);
		multiply();
		push(PY);
		push(X);
		multiply();
		add();

		PY = pop();
		PX = pop();
	}

	// X + i*Y

	push(PX);
	push(imaginaryunit);
	push(PY);
	multiply();
	add();
}

void
power_complex_double(void)
{
	double expo, r, theta, x, y;

	push(EXPO);
	expo = pop_double();

	push(X);
	x = pop_double();

	push(Y);
	y = pop_double();

	r = hypot(x, y);
	theta = atan2(y, x);

	r = pow(r, expo);
	theta = expo * theta;

	x = r * cos(theta);
	y = r * sin(theta);

	push_double(x);
	push_double(y);
	push(imaginaryunit);
	multiply();
	add();
}

// X and Y are rational, EXPO is rational and not an integer

void
power_complex_rational(void)
{
	// calculate sqrt(X^2 + Y^2) ^ (1/2 * EXPO)

	push(X);
	push(X);
	multiply();
	push(Y);
	push(Y);
	multiply();
	add();
	push_rational(1, 2);
	push(EXPO);
	multiply();
	power();

	// calculate (-1) ^ (EXPO * arctan(Y, X) / pi)

	push(Y);
	push(X);
	arctan();
	push_symbol(PI);
	divide();
	push(EXPO);
	multiply();
	EXPO = pop();
	power_imaginary_unit();

	// result = sqrt(X^2 + Y^2) ^ (1/2 * EXPO) * (-1) ^ (EXPO * arctan(Y, X) / pi)

	multiply();
}

// BASE and EXPO are numerical (rational or double)

void
power_numbers(void)
{
	double base, expo;

	if (iszero(BASE) && isnegativenumber(EXPO))
		stop("divide by zero");

	if (equaln(BASE, -1)) {
		power_imaginary_unit();
		return;
	}

	if (isnegativenumber(BASE)) {
		power_imaginary_unit();
		push(BASE);
		negate();
		BASE = pop();
		power_numbers();
		multiply();
		return;
	}

	if (BASE->k == RATIONAL && EXPO->k == RATIONAL) {
		power_rationals();
		return;
	}

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	push_double(pow(base, expo));
}

// BASE and EXPO are rational numbers, BASE is nonnegative

void
power_rationals(void)
{
	int i, j, h, n;
	struct atom **s;
	uint32_t *a, *b;
	uint32_t *base_numer, *base_denom;
	uint32_t *expo_numer, *expo_denom;

	base_numer = BASE->u.q.a;
	base_denom = BASE->u.q.b;

	expo_numer = EXPO->u.q.a;
	expo_denom = EXPO->u.q.b;

	// if EXPO is -1 then return reciprocal of BASE

	if (equaln(EXPO, -1)) {
		a = mcopy(base_numer);
		b = mcopy(base_denom);
		push_rational_number(MPLUS, b, a); // reciprocate
		return;
	}

	// if EXPO is integer then return BASE ^ EXPO

	if (MEQUAL(expo_denom, 1)) {
		a = mpow(base_numer, expo_numer);
		b = mpow(base_denom, expo_numer);
		if (EXPO->sign == MMINUS)
			push_rational_number(MPLUS, b, a); // reciprocate
		else
			push_rational_number(MPLUS, a, b);
		return;
	}

	h = tos;
	s = stack + h;

	// put factors on stack

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);

	factor_factor();

	// normalize factors

	n = tos - h;

	for (i = 0; i < n; i++) {
		p3 = s[i];
		if (car(p3) == symbol(POWER)) {
			BASE = cadr(p3);
			EXPO = caddr(p3);
			power_rationals_nib();
			s[i] = pop(); // trick: fill hole
		}
	}

	// multiply rationals

	p4 = one;

	n = tos - h;

	for (i = 0; i < n; i++) {
		p3 = s[i];
		if (p3->k == RATIONAL) {
			push(p3);
			push(p4);
			multiply();
			p4 = pop();
			for (j = i + 1; j < n; j++)
				s[j - 1] = s[j];
			i--;
			n--;
			tos--;
		}
	}

	// finalize

	if (!equaln(p4, 1))
		push(p4);

	n = tos - h;

	if (n > 1) {
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons();
	}
}

// BASE is an integer, EXPO is an integer or rational number

void
power_rationals_nib(void)
{
	uint32_t *a, *b, *base, *expo_numer, *expo_denom, *t;

	base = BASE->u.q.a;

	expo_numer = EXPO->u.q.a;
	expo_denom = EXPO->u.q.b;

	// integer power?

	if (MEQUAL(expo_denom, 1)) {
		a = mpow(base, expo_numer);
		b = mint(1);
		if (EXPO->sign == MMINUS)
			push_rational_number(MPLUS, b, a); // reciprocate
		else
			push_rational_number(MPLUS, a, b);
		return;
	}

	// evaluate whole part

	if (mcmp(expo_numer, expo_denom) > 0) {
		t = mdiv(expo_numer, expo_denom);
		a = mpow(base, t);
		b = mint(1);
		mfree(t);
		if (EXPO->sign == MMINUS)
			push_rational_number(MPLUS, b, a); // reciprocate
		else
			push_rational_number(MPLUS, a, b);
		// reduce EXPO to fractional part
		a = mmod(expo_numer, expo_denom);
		b = mcopy(expo_denom);
		push_rational_number(EXPO->sign, a, b);
		EXPO = pop();
		expo_numer = EXPO->u.q.a;
		expo_denom = EXPO->u.q.b;
	}

	if (MLENGTH(base) == 1) {
		// base is a prime number from factor_factor()
		push_symbol(POWER);
		a = mcopy(base);
		b = mint(1);
		push_rational_number(MPLUS, a, b);
		push(EXPO);
		list(3);
		return;
	}

	t = mroot(base, expo_denom);

	if (t == NULL) {
		push_symbol(POWER);
		a = mcopy(base);
		b = mint(1);
		push_rational_number(MPLUS, a, b);
		push(EXPO);
		list(3);
		return;
	}

	a = mpow(t, expo_numer);
	b = mint(1);

	mfree(t);

	if (EXPO->sign == MMINUS)
		push_rational_number(MPLUS, b, a); // reciprocate
	else
		push_rational_number(MPLUS, a, b);
}

void
sqrtv(void)
{
	push_rational(1, 2);
	power();
}
