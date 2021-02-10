#include "defs.h"

void
eval_kron(void)
{
	push(cadr(p1));
	eval();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		kron();
		p1 = cdr(p1);
	}
}

void
kron(void)
{
	save();
	kron_nib();
	restore();
}

void
kron_nib(void)
{
	int i, j, k, l, m, n, p, q;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1->u.tensor->ndim > 2 || p2->u.tensor->ndim > 2)
		stop("kron");

	m = p1->u.tensor->dim[0];
	n = p1->u.tensor->ndim == 1 ? 1 : p1->u.tensor->dim[1];

	p = p2->u.tensor->dim[0];
	q = p2->u.tensor->ndim == 1 ? 1 : p2->u.tensor->dim[1];

	p3 = alloc_tensor(m * n * p * q);

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			for (k = 0; k < p; k++)
				for (l = 0; l < q; l++) {
					push(p1->u.tensor->elem[n * i + j]);
					push(p2->u.tensor->elem[q * k + l]);
					multiply();
					p3->u.tensor->elem[n * p * q * i + n * q * k + q * j + l] = pop();
				}

	if (n * q == 1) {
		p3->u.tensor->ndim = 1;
		p3->u.tensor->dim[0] = m * p;
	} else {
		p3->u.tensor->ndim = 2;
		p3->u.tensor->dim[0] = m * p;
		p3->u.tensor->dim[1] = n * q;
	}

	push(p3);
}
