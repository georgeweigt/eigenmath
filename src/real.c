#include "defs.h"

void
eval_real(void)
{
	push(cadr(p1));
	eval();
	real();
}

void
real(void)
{
	save();
	rect();
	p1 = pop();
	push(p1);
	push(p1);
	conjugate();
	add();
	push_rational(1, 2);
	multiply();
	restore();
}
