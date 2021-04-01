#include "defs.h"

void
eval_kronecker(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		kronecker();
		p1 = cdr(p1);
	}
	expanding = t;
}

void
kronecker(void)
{
	save();
	kronecker_nib();
	restore();
}

void
kronecker_nib(void)
{
	int h, i, j, k, l, m, n, p, q;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1->u.tensor->ndim > 2 || p2->u.tensor->ndim > 2)
		stop("kronecker");

	m = p1->u.tensor->dim[0];
	n = p1->u.tensor->ndim == 1 ? 1 : p1->u.tensor->dim[1];

	p = p2->u.tensor->dim[0];
	q = p2->u.tensor->ndim == 1 ? 1 : p2->u.tensor->dim[1];

	p3 = alloc_tensor(m * n * p * q);

	h = 0;

	// result matrix has (m * p) rows and (n * q) columns

	for (i = 0; i < m; i++)
		for (j = 0; j < p; j++)
			for (k = 0; k < n; k++)
				for (l = 0; l < q; l++) {
					push(p1->u.tensor->elem[n * i + k]);
					push(p2->u.tensor->elem[q * j + l]);
					multiply();
					p3->u.tensor->elem[h++] = pop();
				}

	p3->u.tensor->dim[0] = m * p;
	p3->u.tensor->dim[1] = n * q;

	p3->u.tensor->ndim = n * q == 1 ? 1 : 2;

	push(p3);
}
