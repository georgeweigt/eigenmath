#include "defs.h"

void
eval_tanh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	tanhfunc();
	expanding = t;
}

void
tanhfunc(void)
{
	save();
	tanhfunc_nib();
	restore();
}

void
tanhfunc_nib(void)
{
	p1 = pop();

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		sinhfunc();
		push(p1);
		coshfunc();
		divide();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// tanh(-x) = -tanh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		tanhfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCTANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(TANH);
	push(p1);
	list(2);
}
