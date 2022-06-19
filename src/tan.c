#include "defs.h"

void
eval_tan(struct atom *p1)
{
	push(cadr(p1));
	eval();
	tanfunc();
}

void
tanfunc(void)
{
	int n;
	double d;
	struct atom *p1, *p2;

	p1 = pop();

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = tan(d);
		push_double(d);
		return;
	}

	if (isdoublez(p1)) {
		push(p1);
		sinfunc();
		push(p1);
		cosfunc();
		divide();
		return;
	}

	// tan(-x) = -tan(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		tanfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		tanfunc_sum(p1);
		return;
	}

	if (car(p1) == symbol(ARCTAN)) {
		push(cadr(p1));
		push(caddr(p1));
		divide();
		return;
	}

	// n pi ?

	push(p1);
	push_symbol(PI);
	divide();
	p2 = pop();

	if (!isnum(p2)) {
		push_symbol(TAN);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p2)) {
		push(p2);
		d = pop_double();
		d = tan(d * M_PI);
		push_double(d);
		return;
	}

	push(p2); // nonnegative by tan(-x) = -tan(x) above
	push_integer(180);
	multiply();
	p2 = pop();

	if (!isinteger(p2)) {
		push_symbol(TAN);
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
tanfunc_sum(struct atom *p1)
{
	struct atom *p2, *p3;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push(car(p2));
		push_symbol(PI);
		divide();
		p3 = pop();
		if (isinteger(p3)) {
			push(p1);
			push(car(p2));
			subtract();
			tanfunc();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(TAN);
	push(p1);
	list(2);
}
