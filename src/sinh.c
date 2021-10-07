#include "defs.h"

void
eval_sinh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	sinhfunc();
	expanding = t;
}

void
sinhfunc(void)
{
	save();
	sinhfunc_nib();
	restore();
}

void
sinhfunc_nib(void)
{
	p1 = pop();

	// sinh(z) = 1/2 (exp(z) - exp(-z))

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
