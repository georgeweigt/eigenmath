#include "defs.h"

// define BASE p8 (defs1.h)
// define EXPO p9 (defs1.h)

#undef R
#define R p3

void
power_natural_number(void)
{
	save();
	power_natural_number_nib();
	restore();
}

void
power_natural_number_nib(void)
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
		cosfunc();
		push(imaginaryunit);
		push_double(y);
		sinfunc();
		multiply();
		add();
		multiply();
		return;
	}

	// e^log(expr) = expr

	if (car(EXPO) == symbol(LOG)) {
		push(cadr(EXPO));
		return;
	}

	if (isdenormalpolar(EXPO)) {
		normalize_polar();
		return;
	}

	push_symbol(POWER);
	push_symbol(EXP1);
	push(EXPO);
	list(3);
}

void
normalize_polar(void)
{
	int h;
	if (car(EXPO) == symbol(ADD)) {
		h = tos;
		p1 = cdr(EXPO);
		while (iscons(p1)) {
			EXPO = car(p1);
			if (isdenormalpolarterm(EXPO))
				normalize_polar_term();
			else {
				push_symbol(POWER);
				push_symbol(EXP1);
				push(EXPO);
				list(3);
			}
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
	} else
		normalize_polar_term();
}

void
normalize_polar_term(void)
{
	// exp(i pi) = -1

	if (length(EXPO) == 3) {
		push_integer(-1);
		return;
	}

	R = cadr(EXPO); // R = coeff of term

	if (isrational(R))
		normalize_polar_term_rational();
	else
		normalize_polar_term_double();
}

void
normalize_polar_term_rational(void)
{
	int n;

	// R = R mod 2

	push(R);
	push_integer(2);
	modfunc();
	R = pop();

	// convert negative rotation to positive

	if (R->sign == MMINUS) {
		push(R);
		push_integer(2);
		add();
		R = pop();
	}

	push(R);
	push_integer(2);
	multiply();
	floorfunc();
	n = pop_integer(); // number of 90 degree turns

	push(R);
	push_integer(n);
	push_rational(-1, 2);
	multiply();
	add();
	R = pop(); // remainder

	switch (n) {

	case 0:
		if (iszero(R))
			push_integer(1);
		else {
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
		}
		break;

	case 1:
		if (iszero(R))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 2:
		if (iszero(R))
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (iszero(R)) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(4);
		}
		break;
	}
}

void
normalize_polar_term_double(void)
{
	double coeff, n, r;

	coeff = R->u.d;

	// coeff = coeff mod 2

	coeff = fmod(coeff, 2.0);

	// convert negative rotation to positive

	if (coeff < 0.0)
		coeff += 2.0;

	n = floor(2.0 * coeff); // number of 90 degree turns

	r = coeff - n / 2.0; // remainder

	switch ((int) n) {

	case 0:
		if (r == 0.0)
			push_integer(1);
		else {
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
		}
		break;

	case 1:
		if (r == 0.0)
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 2:
		if (r == 0.0)
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (r == 0.0) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(4);
		}
		break;
	}
}
