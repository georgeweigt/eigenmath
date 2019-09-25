#include "defs.h"

void
eval_cosh(void)
{
	push(cadr(p1));
	eval();
	cosineh();
}

void
cosineh(void)
{
	save();
	cosineh_nib();
	restore();
}

void
cosineh_nib(void)
{
	p1 = pop();

	// cosh(z) = 1/2 (exp(z) + exp(-z))

	if (isdouble(p1) || isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		exponential();
		push(p1);
		negate();
		exponential();
		add();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push(one);
		return;
	}

	// cosh(-x) = cosh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		cosineh();
		return;
	}

	if (car(p1) == symbol(ARCCOSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(COSH);
	push(p1);
	list(2);
}
