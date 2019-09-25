#include "defs.h"

void
eval_cofactor(void)
{
	int col, i, j, k, n, row;

	push(cadr(p1));
	eval();
	p2 = pop();

	if (!istensor(p2) || p2->u.tensor->ndim != 2 || p2->u.tensor->dim[0] != p2->u.tensor->dim[1])
		stop("square matrix expected");

	n = p2->u.tensor->dim[0];

	push(caddr(p1));
	eval();
	row = pop_integer();

	push(cadddr(p1));
	eval();
	col = pop_integer();

	if (row < 1 || row > n || col < 1 || col > n)
		stop("index err");

	row--; // make zero based
	col--;

	p3 = alloc_tensor((n - 1) * (n - 1));
	p3->u.tensor->ndim = 2;
	p3->u.tensor->dim[0] = n - 1;
	p3->u.tensor->dim[1] = n - 1;

	k = 0;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i != row && j != col)
				p3->u.tensor->elem[k++] = p2->u.tensor->elem[n * i + j];

	push(p3);
	det();

	if ((row + col) % 2)
		negate();
}
