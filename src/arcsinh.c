#include "defs.h"

void
eval_arcsinh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	arcsinh();
	expanding = t;
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

	if (isdouble(p1)) {
		push_double(asinh(p1->u.d));
		return;
	}

	// arcsinh(z) = log(sqrt(z^2 + 1) + z)

	if (isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(1.0);
		add();
		sqrtfunc();
		push(p1);
		add();
		logfunc();
		return;
	}

	if (iszero(p1)) {
		push(p1);
		return;
	}

	// arcsinh(-x) = -arcsinh(x)

	if (isnegativeterm(p1)) {
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
