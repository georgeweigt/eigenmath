#include "defs.h"

void
eval_erf(void)
{
	push(cadr(p1));
	eval();
	erf_symbolic();
}

void
erf_symbolic(void)
{
	save();
	erf_symbolic_nib();
	restore();
}

void
erf_symbolic_nib(void)
{
	double d;

	p1 = pop();

	if (isdouble(p1)) {
		d = 1.0 - erfc(p1->u.d);
		push_double(d);
		return;
	}

	if (isnegativeterm(p1)) {
		push_symbol(ERF);
		push(p1);
		negate();
		list(2);
		negate();
		return;
	}

	push_symbol(ERF);
	push(p1);
	list(2);
	return;
}
