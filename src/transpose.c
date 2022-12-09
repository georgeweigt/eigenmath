void
eval_transpose(struct atom *p1)
{
	int m, n;
	struct atom *p2;

	push(cadr(p1));
	eval();
	p2 = pop();
	push(p2);

	if (!istensor(p2) || p2->u.tensor->ndim < 2)
		return;

	p1 = cddr(p1);

	if (!iscons(p1)) {
		transpose(1, 2);
		return;
	}

	while (iscons(p1)) {

		push(car(p1));
		eval();
		n = pop_integer();

		push(cadr(p1));
		eval();
		m = pop_integer();

		transpose(n, m);

		p1 = cddr(p1);
	}
}

void
transpose(int n, int m)
{
	int i, j, k, ndim, nelem;
	int index[MAXDIM];
	struct atom *p1, *p2;

	p1 = pop();

	ndim = p1->u.tensor->ndim;
	nelem = p1->u.tensor->nelem;

	if (n < 1 || n > ndim || m < 1 || m > ndim)
		stop("transpose: index error");

	n--; // make zero based
	m--;

	p2 = copy_tensor(p1);

	// interchange indices n and m

	p2->u.tensor->dim[n] = p1->u.tensor->dim[m];
	p2->u.tensor->dim[m] = p1->u.tensor->dim[n];

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

		p2->u.tensor->elem[k] = p1->u.tensor->elem[i];

		// increment index

		for (j = ndim - 1; j >= 0; j--) {
			if (++index[j] < p1->u.tensor->dim[j])
				break;
			index[j] = 0;
		}
	}

	push(p2);
}
