#include "defs.h"

void
eval_sinh(void)
{
	push(cadr(p1));
	eval();
	sineh();
}

void
sineh(void)
{
	save();
	sineh_nib();
	restore();
}

void
sineh_nib(void)
{
	p1 = pop();

	// sinh(z) = 1/2 (exp(z) - exp(-z))

	if (isdouble(p1) || isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		exponential();
		push(p1);
		negate();
		exponential();
		subtract();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push(zero);
		return;
	}

	// sinh(-x) = -sinh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		sineh();
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
