#include "defs.h"

void
eval_adj(struct atom *p1)
{
	push(cadr(p1));
	eval();
	adj();
}

void
adj(void)
{
	int col, i, j, k, n, row;
	struct atom *p1, *p2, *p3;

	p1 = pop();

	if (!istensor(p1)) {
		push_integer(1); // adj of scalar is 1 because adj = det inv
		return;
	}

	if (p1->u.tensor->ndim != 2 || p1->u.tensor->dim[0] != p1->u.tensor->dim[1])
		stop("adj");

	n = p1->u.tensor->dim[0];

	// p2 is the adjunct matrix

	p2 = alloc_matrix(n, n);

	if (n == 2) {
		p2->u.tensor->elem[0] = p1->u.tensor->elem[3];
		push(p1->u.tensor->elem[1]);
		negate();
		p2->u.tensor->elem[1] = pop();
		push(p1->u.tensor->elem[2]);
		negate();
		p2->u.tensor->elem[2] = pop();
		p2->u.tensor->elem[3] = p1->u.tensor->elem[0];
		push(p2);
		return;
	}

	// p3 is for computing cofactors

	p3 = alloc_matrix(n - 1, n - 1);

	for (row = 0; row < n; row++) {
		for (col = 0; col < n; col++) {
			k = 0;
			for (i = 0; i < n; i++)
				for (j = 0; j < n; j++)
					if (i != row && j != col)
						p3->u.tensor->elem[k++] = p1->u.tensor->elem[n * i + j];
			push(p3);
			det();
			if ((row + col) % 2)
				negate();
			p2->u.tensor->elem[n * col + row] = pop(); // transpose
		}
	}

	push(p2);
}
