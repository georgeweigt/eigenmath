void
eval_minormatrix(struct atom *p1)
{
	int i, j;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	push(caddr(p1));
	evalf();
	i = pop_integer();

	push(cadddr(p1));
	evalf();
	j = pop_integer();

	if (!istensor(p2) || p2->u.tensor->ndim != 2)
		stopf("minormatrix");

	if (i < 1 || i > p2->u.tensor->dim[0] || j < 0 || j > p2->u.tensor->dim[1])
		stopf("minormatrix");

	push(p2);

	minormatrix(i, j);
}

void
minormatrix(int row, int col)
{
	int i, j, k, m, n;
	struct atom *p1, *p2;

	p2 = symbol(NIL); // silence compiler

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
