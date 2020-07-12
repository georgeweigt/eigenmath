#include "defs.h"

void
eval_tan(void)
{
	push(cadr(p1));
	eval();
	stan();
}

void
stan(void)
{
	save();
	stan_nib();
	restore();
}

void
stan_nib(void)
{
	int n;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(tan(p1->u.d));
		return;
	}

	if (isdoublez(p1)) {
		push(p1);
		ssin();
		push(p1);
		scos();
		divide();
		return;
	}

	// tan(-x) = -tan(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		stan();
		negate();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		stan_of_sum();
		return;
	}

	if (car(p1) == symbol(ARCTAN)) {
		push(cadr(p1));
		push(caddr(p1));
		divide();
		return;
	}

	// multiply by 180/pi

	push(p1); // nonnegative by code above
	push_integer(180);
	multiply();
	push_symbol(PI);
	divide();

	n = pop_integer();

	if (n == ERR) {
		push_symbol(TAN);
		push(p1);
		list(2);
		return;
	}

	switch (n % 360) {
	case 0:
	case 180:
		push_integer(0);
		break;
	case 30:
	case 210:
		push_rational(1, 3);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 150:
	case 330:
		push_rational(-1, 3);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 45:
	case 225:
		push_integer(1);
		break;
	case 135:
	case 315:
		push_integer(-1);
		break;
	case 60:
	case 240:
		push_integer(3);
		push_rational(1, 2);
		power();
		break;
	case 120:
	case 300:
		push_integer(3);
		push_rational(1, 2);
		power();
		negate();
		break;
	default:
		push_symbol(TAN);
		push(p1);
		list(2);
		break;
	}
}

// tan(x + n pi) = tan(x)

void
stan_of_sum(void)
{
	int n;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push(car(p2));
		push_symbol(PI);
		divide();
		n = pop_integer();
		if (n != ERR) {
			push(p1);
			push(car(p2));
			subtract();
			stan();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(TAN);
	push(p1);
	list(2);
}
