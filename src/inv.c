#include "defs.h"

void
eval_inv(void)
{
	push(cadr(p1));
	eval();
	inv();
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
	int t;

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

	// ensure inv(A) gives the same result as adj(A)/det(A)

	push(p1);
	t = expanding;
	expanding = 0;
	det();
	expanding = t;

	divide();
}
