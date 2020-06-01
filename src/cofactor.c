#include "defs.h"

void
eval_cofactor(void)
{
	int col, i, j, k, n, row;

	push(cadr(p1));
	eval();
	p2 = pop();

	if (!istensor(p2) || p2->u.tensor->ndim != 2 || p2->u.tensor->dim[0] != p2->u.tensor->dim[1])
		stop("cofactor: square matrix expected");

	n = p2->u.tensor->dim[0];

	push(caddr(p1));
	eval();
	row = pop_integer();

	push(cadddr(p1));
	eval();
	col = pop_integer();

	if (row < 1 || row > n || col < 1 || col > n)
		stop("cofactor: index range error");

	row--; // make zero based
	col--;

	if (n == 2) {
		push(p2);
		adj();
		p3 = pop();
		push(p3->u.tensor->elem[n * col + row]);
		return;
	}

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
