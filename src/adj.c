#include "defs.h"

void
eval_adj(void)
{
	push(cadr(p1));
	eval();
	adj();
}

void
adj(void)
{
	save();
	adj_nib();
	restore();
}

void
adj_nib(void)
{
	int i, j, n;

	p1 = pop();

	if (!istensor(p1) || p1->u.tensor->ndim != 2 || p1->u.tensor->dim[0] != p1->u.tensor->dim[1])
		stop("adj: square matrix expected");

	n = p1->u.tensor->dim[0];

	// p3 is the adjunct matrix

	push(p1);
	copy_tensor();
	p3 = pop();

	if (n == 2) {
		p3->u.tensor->elem[0] = p1->u.tensor->elem[3];
		p3->u.tensor->elem[3] = p1->u.tensor->elem[0];
		push(p1->u.tensor->elem[1]);
		negate();
		p3->u.tensor->elem[1] = pop();
		push(p1->u.tensor->elem[2]);
		negate();
		p3->u.tensor->elem[2] = pop();
		push(p3);
		return;
	}

	// p2 is for computing cofactors

	p2 = alloc_tensor((n - 1) * (n - 1));
	p2->u.tensor->ndim = 2;
	p2->u.tensor->dim[0] = n - 1;
	p2->u.tensor->dim[1] = n - 1;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++) {
			adj_cofactor(n, i, j);
			p3->u.tensor->elem[n * j + i] = pop(); // transpose
		}

	push(p3);
}

void
adj_cofactor(int n, int row, int col)
{
	int i, j, k = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i != row && j != col)
				p2->u.tensor->elem[k++] = p1->u.tensor->elem[n * i + j];
	push(p2);
	det();
	if ((row + col) % 2)
		negate();
}
