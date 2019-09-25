#include "defs.h"

void
eval_outer(void)
{
	p1 = cdr(p1);
	push(car(p1));
	eval();
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		outer();
		p1 = cdr(p1);
	}
}

void
outer(void)
{
	save();

	p2 = pop();
	p1 = pop();

	if (istensor(p1) && istensor(p2)) {
		outer_nib();
		restore();
		return;
	}

	if (istensor(p1)) {
		push(p2); // swap order
		push(p1);
		restore();
		scalar_times_tensor();
		return;
	}

	if (istensor(p2)) {
		push(p1);
		push(p2);
		restore();
		scalar_times_tensor();
		return;
	}

	push(p1);
	push(p2);
	restore();
	multiply();
}

// outer product of p1 and p2

void
outer_nib(void)
{
	int i, j, k, ncol, ndim, nrow;
	struct atom **a, **b, **c;

	ndim = p1->u.tensor->ndim + p2->u.tensor->ndim;

	if (ndim > MAXDIM)
		stop("rank exceeds max");

	nrow = p1->u.tensor->nelem;
	ncol = p2->u.tensor->nelem;

	p3 = alloc_tensor(nrow * ncol);

	a = p1->u.tensor->elem;
	b = p2->u.tensor->elem;
	c = p3->u.tensor->elem;

	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++) {
			push(a[i]);
			push(b[j]);
			multiply();
			c[i * ncol + j] = pop();
		}

	// add dim info

	p3->u.tensor->ndim = ndim;

	k = 0;

	for (i = 0; i < p1->u.tensor->ndim; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	for (i = 0; i < p2->u.tensor->ndim; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	push(p3);
}
