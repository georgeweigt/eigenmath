#include "defs.h"

#undef X
#undef Y

#define X p5
#define Y p6

void
eval_sin(void)
{
	push(cadr(p1));
	eval();
	ssin();
}

void
ssin(void)
{
	save();
	ssin_nib();
	restore();
}

void
ssin_nib(void)
{
	int n;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(sin(p1->u.d));
		return;
	}

	// 0?

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// sin(z) = -i/2 (exp(i z) - exp(-i z))

	if (isdoublez(p1)) {
		push_double(-0.5);
		push(imaginaryunit);
		multiply();
		push(imaginaryunit);
		push(p1);
		multiply();
		exponential();
		push(imaginaryunit);
		negate();
		push(p1);
		multiply();
		exponential();
		subtract();
		multiply();
		return;
	}

	// sin(-x) = -sin(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		ssin();
		negate();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		ssin_of_sum();
		return;
	}

	// sin(arctan(y,x)) = y (x^2 + y^2)^(-1/2)

	if (car(p1) == symbol(ARCTAN)) {
		X = caddr(p1);
		Y = cadr(p1);
		push(Y);
		push(X);
		push(X);
		multiply();
		push(Y);
		push(Y);
		multiply();
		add();
		push_rational(-1, 2);
		power();
		multiply();
		return;
	}

	// sin(arccos(x)) = sqrt(1 - x^2)

	if (car(p1) == symbol(ARCCOS)) {
		push_integer(1);
		push(cadr(p1));
		push_integer(2);
		power();
		subtract();
		push_rational(1, 2);
		power();
		return;
	}

	// n pi ?

	push(p1);
	push_symbol(PI);
	divide();
	p2 = pop();

	if (!isnum(p2)) {
		push_symbol(SIN);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p2)) {
		push_double(sin(p2->u.d * M_PI));
		return;
	}

	push(p2); // nonnegative by sin(-x) = -sin(x) above
	push_integer(180);
	multiply();
	p2 = pop();

	if (!isinteger(p2)) {
		push_symbol(SIN);
		push(p1);
		list(2);
		return;
	}

	push(p2);
	push_integer(360);
	modfunc();
	n = pop_integer();

	switch (n) {
	case 0:
	case 180:
		push_integer(0);
		break;
	case 30:
	case 150:
		push_rational(1, 2);
		break;
	case 210:
	case 330:
		push_rational(-1, 2);
		break;
	case 45:
	case 135:
		push_rational(1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 225:
	case 315:
		push_rational(-1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 60:
	case 120:
		push_rational(1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 240:
	case 300:
		push_rational(-1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 90:
		push_integer(1);
		break;
	case 270:
		push_integer(-1);
		break;
	default:
		push_symbol(SIN);
		push(p1);
		list(2);
		break;
	}
}

// sin(x + n/2 pi) = sin(x) cos(n/2 pi) + cos(x) sin(n/2 pi)

void
ssin_of_sum(void)
{
	int n;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push_integer(2);
		push(car(p2));
		multiply();
		push_symbol(PI);
		divide();
		n = pop_integer();
		if (n != ERR) {
			push(p1);
			push(car(p2));
			subtract();
			p3 = pop();
			push(p3);
			ssin();
			push(car(p2));
			scos();
			multiply();
			push(p3);
			scos();
			push(car(p2));
			ssin();
			multiply();
			add();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(SIN);
	push(p1);
	list(2);
}
