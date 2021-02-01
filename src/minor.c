#include "defs.h"

void
eval_minor(void)
{
	int i, j;

	push(cadr(p1));
	eval();
	p2 = pop();

	push(caddr(p1));
	eval();
	i = pop_integer();

	push(cadddr(p1));
	eval();
	j = pop_integer();

	if (!istensor(p2) || p2->u.tensor->ndim != 2 || p2->u.tensor->dim[0] != p2->u.tensor->dim[1])
		stop("minor");

	if (i < 1 || i > p2->u.tensor->dim[0] || j < 0 || j > p2->u.tensor->dim[1])
		stop("minor");

	push(p2);

	minormatrix(i, j);

	det();
}

void
eval_minormatrix(void)
{
	int i, j;

	push(cadr(p1));
	eval();
	p2 = pop();

	push(caddr(p1));
	eval();
	i = pop_integer();

	push(cadddr(p1));
	eval();
	j = pop_integer();

	if (!istensor(p2) || p2->u.tensor->ndim != 2)
		stop("minormatrix");

	if (i < 1 || i > p2->u.tensor->dim[0] || j < 0 || j > p2->u.tensor->dim[1])
		stop("minormatrix");

	push(p2);

	minormatrix(i, j);
}

void
minormatrix(int row, int col)
{
	save();
	minormatrix_nib(row, col);
	restore();
}

void
minormatrix_nib(int row, int col)
{
	int i, j, k, m, n;

	p1 = pop();

	n = p1->u.tensor->dim[0];
	m = p1->u.tensor->dim[1];

	if (n == 2 && m == 2) {
		if (row == 1) {
			if (col == 1)
				push(p1->u.tensor->elem[3]);
			else
				push(p1->u.tensor->elem[2]);
		} else {
			if (col == 1)
				push(p1->u.tensor->elem[1]);
			else
				push(p1->u.tensor->elem[0]);
		}
		return;
	}

	if (n == 2)
		p2 = alloc_vector(m - 1);

	if (m == 2)
		p2 = alloc_vector(n - 1);

	if (n > 2 && m > 2)
		p2 = alloc_matrix(n - 1, m - 1);

	row--;
	col--;

	k = 0;

	for (i = 0; i < n; i++) {

		if (i == row)
			continue;

		for (j = 0; j < m; j++) {

			if (j == col)
				continue;

			p2->u.tensor->elem[k++] = p1->u.tensor->elem[m * i + j];
		}
	}

	push(p2);
}
