#include "defs.h"

void
eval_abs(void)
{
	push(cadr(p1));
	eval();
	absv();
}

void
absv(void)
{
	save();
	absv_nib();
	restore();
}

void
absv_nib(void)
{
	p1 = pop();

	if (istensor(p1)) {
		if (p1->u.tensor->ndim != 1)
			stop("abs");
		push(p1);
		push(p1);
		conjugate();
		inner();
		sqrtv();
		return;
	}

	push(p1);
	push(p1);
	conjugate();
	multiply();
	sqrtv();
}
