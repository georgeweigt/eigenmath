#include "defs.h"

void
eval_det(void)
{
	push(cadr(p1));
	eval();
	det();
}

void
det(void)
{
	save();
	det_nib();
	restore();
}

void
det_nib(void)
{
	int h, i, j, k, m, n;

	p1 = pop();

	if (!istensor(p1) || p1->u.tensor->ndim != 2 || p1->u.tensor->dim[0] != p1->u.tensor->dim[1])
		stop("det: square matrix expected");

	n = p1->u.tensor->dim[0];

	switch (n) {
	case 1:
		push(p1->u.tensor->elem[0]);
		return;
	case 2:
		push(p1->u.tensor->elem[0]);
		push(p1->u.tensor->elem[3]);
		multiply();
		push(minusone);
		push(p1->u.tensor->elem[1]);
		push(p1->u.tensor->elem[2]);
		multiply_factors(3);
		add();
		return;
	}

	p2 = alloc_tensor((n - 1) * (n - 1));
	p2->u.tensor->ndim = 2;
	p2->u.tensor->dim[0] = n - 1;
	p2->u.tensor->dim[1] = n - 1;

	h = tos;

	for (m = 0; m < n; m++) {
		if (iszero(p1->u.tensor->elem[m]))
			continue;
		k = 0;
		for (i = 1; i < n; i++)
			for (j = 0; j < n; j++)
				if (j != m)
					p2->u.tensor->elem[k++] = p1->u.tensor->elem[n * i + j];
		push(p2);
		det();
		push(p1->u.tensor->elem[m]);
		multiply();
		if (m % 2 == 1)
			negate();
	}

	if (h == tos)
		push(zero);
	else
		add_terms(tos - h);
}
