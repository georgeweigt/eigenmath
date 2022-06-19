#include "defs.h"

void
eval_sinh(struct atom *p1)
{
	push(cadr(p1));
	eval();
	sinhfunc();
}

void
sinhfunc(void)
{
	double d;
	struct atom *p1;

	p1 = pop();

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = sinh(d);
		push_double(d);
		return;
	}

	// sinh(z) = 1/2 exp(z) - 1/2 exp(-z)

	if (isdouble(p1) || isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		expfunc();
		push(p1);
		negate();
		expfunc();
		subtract();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// sinh(-x) -> -sinh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		sinhfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCSINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(SINH);
	push(p1);
	list(2);
}
