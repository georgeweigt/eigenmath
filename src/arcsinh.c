#include "defs.h"

void
eval_arcsinh(void)
{
	push(cadr(p1));
	eval();
	arcsinh();
}

void
arcsinh(void)
{
	save();
	arcsinh_nib();
	restore();
}

void
arcsinh_nib(void)
{
	p1 = pop();

	// arcsinh(z) = log(z + (z^2 + 1)^(1/2))

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(1.0);
		add();
		push_rational(1, 2);
		power();
		push(p1);
		add();
		logarithm();
		return;
	}

	if (iszero(p1)) {
		push(p1);
		return;
	}

	// arcsinh(-x) = -arcsinh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		arcsinh();
		negate();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCSINH);
	push(p1);
	list(2);
}
