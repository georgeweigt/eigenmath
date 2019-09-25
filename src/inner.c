#include "defs.h"

void
eval_inner(void)
{
	p1 = cdr(p1);
	push(car(p1));
	eval();
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		inner();
		p1 = cdr(p1);
	}
}

void
inner(void)
{
	save();

	p2 = pop();
	p1 = pop();

	if (istensor(p1) && istensor(p2)) {
		inner_nib();
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

// inner product of p1 and p2

void
inner_nib(void)
{
	int i, j, k, mcol, mrow, ncol, ndim, nrow;
	struct atom **a, **b, **c;

	k = p1->u.tensor->ndim - 1;

	ncol = p1->u.tensor->dim[k];
	mrow = p2->u.tensor->dim[0];

	if (ncol != mrow)
		stop("tensor dimensions");

	ndim = p1->u.tensor->ndim + p2->u.tensor->ndim - 2;

	if (ndim > MAXDIM)
		stop("rank exceeds max");

	//	nrow is the number of rows in p1
	//
	//	mcol is the number of columns p2
	//
	//	Example:
	//
	//	A[3][3][4] B[4][4][3]
	//
	//	  3  3				nrow = 3 * 3 = 9
	//
	//	                4  3		mcol = 4 * 3 = 12

	nrow = p1->u.tensor->nelem / ncol;
	mcol = p2->u.tensor->nelem / mrow;

	p3 = alloc_tensor(nrow * mcol);

	a = p1->u.tensor->elem;
	b = p2->u.tensor->elem;
	c = p3->u.tensor->elem;

	for (i = 0; i < nrow; i++) {
		for (j = 0; j < mcol; j++) {
			for (k = 0; k < ncol; k++) {
				push(a[i * ncol + k]);
				push(b[k * mcol + j]);
				multiply();
			}
			add_terms(ncol);
			c[i * mcol + j] = pop();
		}
	}

	if (ndim == 0) {
		push(c[0]);
		return;
	}

	// add dim info

	p3->u.tensor->ndim = ndim;

	k = 0;

	for (i = 0; i < p1->u.tensor->ndim - 1; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	for (i = 1; i < p2->u.tensor->ndim; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	push(p3);
}
