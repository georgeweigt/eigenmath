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

	p1 = pop();

	if (!istensor(p1) || p1->u.tensor->ndim != 2 || p1->u.tensor->dim[0] != p1->u.tensor->dim[1])
		stop("inv: square matrix expected");

	push(p1);
	adj();
	push(p1);
	det();
	p2 = pop();
	if (iszero(p2))
		stop("inv: singular matrix");
	push(p2);
	divide();

	restore();
}
