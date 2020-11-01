#include "defs.h"

void
eval_transpose(void)
{
	push(cadr(p1));
	eval();
	p2 = pop();
	push(p2);

	if (!istensor(p2) || p2->u.tensor->ndim == 1)
		return; // scalar or vector

	p1 = cddr(p1);

	if (!iscons(p1)) {
		push_integer(1);
		push_integer(2);
		transpose();
		return;
	}

	while (iscons(p1)) {
		push(car(p1));
		eval();
		push(cadr(p1));
		eval();
		transpose();
		p1 = cddr(p1);
	}
}

void
transpose(void)
{
	save();
	transpose_nib();
	restore();
}

void
transpose_nib(void)
{
	int i, j, k, m, n, ndim, nelem;
	int index[MAXDIM];
	struct atom **a, **b;

	p3 = pop();
	p2 = pop();
	p1 = pop();

	ndim = p1->u.tensor->ndim;

	nelem = p1->u.tensor->nelem;

	push(p2);
	n = pop_integer();

	push(p3);
	m = pop_integer();

	if (n < 1 || n > ndim || m < 1 || m > ndim)
		stop("transpose: index error");

	n--; // make zero based
	m--;

	push(p1);
	copy_tensor();
	p2 = pop();

	// interchange indices n and m

	p2->u.tensor->dim[n] = p1->u.tensor->dim[m];
	p2->u.tensor->dim[m] = p1->u.tensor->dim[n];

	// copy from a to b

	a = p1->u.tensor->elem;
	b = p2->u.tensor->elem;

	for (i = 0; i < ndim; i++)
		index[i] = 0;

	for (i = 0; i < nelem; i++) {

		k = 0;

		for (j = 0; j < ndim; j++) {
			if (j == n)
				k = k * p1->u.tensor->dim[m] + index[m];
			else if (j == m)
				k = k * p1->u.tensor->dim[n] + index[n];
			else
				k = k * p1->u.tensor->dim[j] + index[j];
		}

		b[k] = a[i];

		// increment index

		for (j = ndim - 1; j >= 0; j--) {
			if (++index[j] < p1->u.tensor->dim[j])
				break;
			index[j] = 0;
		}
	}

	push(p2);
}
