#include "defs.h"

void
eval_arctanh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	arctanh();
	expanding = t;
}

void
arctanh(void)
{
	save();
	arctanh_nib();
	restore();
}

void
arctanh_nib(void)
{
	p1 = pop();

	if (isdouble(p1)) {
		push_double(atanh(p1->u.d));
		return;
	}

	// arctanh(z) = 1/2 log(1 + z) - 1/2 log(1 - z)

	if (isdoublez(p1)) {
		push_double(1.0);
		push(p1);
		add();
		logfunc();
		push_double(1.0);
		push(p1);
		subtract();
		logfunc();
		subtract();
		push_double(0.5);
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// arctanh(-x) = -arctanh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		arctanh();
		negate();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCTANH);
	push(p1);
	list(2);
}
