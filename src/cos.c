#include "defs.h"

#undef X
#undef Y

#define X p5
#define Y p6

void
eval_cos(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	cosfunc();
	expanding = t;
}

void
cosfunc(void)
{
	save();
	cosfunc_nib();
	restore();
}

void
cosfunc_nib(void)
{
	int n;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(cos(p1->u.d));
		return;
	}

	// 0?

	if (iszero(p1)) {
		push_integer(1);
		return;
	}

	// cos(z) = 1/2 (exp(i z) + exp(-i z))

	if (isdoublez(p1)) {
		push_double(0.5);
		push(imaginaryunit);
		push(p1);
		multiply();
		expfunc();
		push(imaginaryunit);
		negate();
		push(p1);
		multiply();
		expfunc();
		add();
		multiply();
		return;
	}

	// cos(-x) = cos(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		cosfunc();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		cosfunc_sum();
		return;
	}

	// cos(arctan(y,x)) = x (x^2 + y^2)^(-1/2)

	if (car(p1) == symbol(ARCTAN)) {
		X = caddr(p1);
		Y = cadr(p1);
		push(X);
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

	// cos(arcsin(x)) = sqrt(1 - x^2)

	if (car(p1) == symbol(ARCSIN)) {
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
		push_symbol(COS);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p2)) {
		push_double(cos(p2->u.d * M_PI));
		return;
	}

	push(p2); // nonnegative by cos(-x) = cos(x) above
	push_integer(180);
	multiply();
	p2 = pop();

	if (!isinteger(p2)) {
		push_symbol(COS);
		push(p1);
		list(2);
		return;
	}

	push(p2);
	push_integer(360);
	modfunc();
	n = pop_integer();

	switch (n) {
	case 90:
	case 270:
		push_integer(0);
		break;
	case 60:
	case 300:
		push_rational(1, 2);
		break;
	case 120:
	case 240:
		push_rational(-1, 2);
		break;
	case 45:
	case 315:
		push_rational(1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 135:
	case 225:
		push_rational(-1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 30:
	case 330:
		push_rational(1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 150:
	case 210:
		push_rational(-1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 0:
		push_integer(1);
		break;
	case 180:
		push_integer(-1);
		break;
	default:
		push_symbol(COS);
		push(p1);
		list(2);
		break;
	}
}

// cos(x + n/2 pi) = cos(x) cos(n/2 pi) - sin(x) sin(n/2 pi)

void
cosfunc_sum(void)
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
			cosfunc();
			push(car(p2));
			cosfunc();
			multiply();
			push(p3);
			sinfunc();
			push(car(p2));
			sinfunc();
			multiply();
			subtract();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(COS);
	push(p1);
	list(2);
}
