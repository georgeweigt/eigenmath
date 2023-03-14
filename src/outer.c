void
eval_outer(struct atom *p1)
{
	push(cadr(p1));
	eval();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		outer();
		p1 = cdr(p1);
	}
}

void
outer(void)
{
	int i, j, k, n, ncol, nrow;
	struct atom *p1, *p2, *p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1->u.tensor->ndim + p2->u.tensor->ndim > MAXDIM)
		stopf("outer: rank exceeds max");

	// sync diffs

	nrow = p1->u.tensor->nelem;
	ncol = p2->u.tensor->nelem;

	p3 = alloc_tensor(nrow * ncol);

	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++) {
			push(p1->u.tensor->elem[i]);
			push(p2->u.tensor->elem[j]);
			multiply();
			p3->u.tensor->elem[i * ncol + j] = pop();
		}

	// dim info

	p3->u.tensor->ndim = p1->u.tensor->ndim + p2->u.tensor->ndim;

	k = 0;

	n = p1->u.tensor->ndim;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	n = p2->u.tensor->ndim;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	push(p3);
}
