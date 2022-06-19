#include "defs.h"

void
eval_outer(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

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
	int i, j, k, n, ncol, ndim, nrow;
	struct atom **a, **b, **c, *p1, *p2, *p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) && !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (istensor(p1) && !istensor(p2)) {
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (!istensor(p1) && istensor(p2)) {
		push(p2);
		copy_tensor();
		p2 = pop();
		n = p2->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1);
			push(p2->u.tensor->elem[i]);
			multiply();
			p2->u.tensor->elem[i] = pop();
		}
		push(p2);
		return;
	}

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
