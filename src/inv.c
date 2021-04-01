#include "defs.h"

void
eval_inv(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	inv();
	expanding = t;
}

void
inv(void)
{
	save();
	inv_nib();
	restore();
}

void
inv_nib(void)
{
	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		reciprocate();
		return;
	}

	if (p1->u.tensor->ndim != 2 || p1->u.tensor->dim[0] != p1->u.tensor->dim[1])
		stop("inv");

	push(p1);
	adj();

	push(p1);
	det();

	divide();
}
