#include "defs.h"

// define BASE p8 (defs1.h)
// define EXPO p9 (defs1.h)

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

// BASE is rectangular complex numerical, EXPO is numerical

void
power_complex_number(void)
{
	save();
	power_complex_number_nib();
	restore();
}

void
power_complex_number_nib(void)
{
	int n;

	// prefixform(2+3*i) = (add 2 (multiply 3 (power -1 1/2)))

	// prefixform(1+i) = (add 1 (power -1 1/2))

	// prefixform(3*i) = (multiply 3 (power -1 1/2))

	// prefixform(i) = (power -1 1/2)

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

	if (!issmallinteger(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	push(EXPO);
	n = pop_integer();

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
	power_minusone();

	// result = sqrt(X^2 + Y^2) ^ (1/2 * EXPO) * (-1) ^ (EXPO * arctan(Y, X) / pi)

	multiply();
}
