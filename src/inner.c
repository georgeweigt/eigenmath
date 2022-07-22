void
eval_inner(struct atom *p1)
{
	int h = tos;

	// evaluate right to left

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		p1 = cdr(p1);
	}

	if (h == tos)
		stop("dot");

	eval();

	while (tos - h > 1) {
		p1 = pop();
		eval();
		push(p1);
		inner();
	}
}

void
inner(void)
{
	int i, j, k, n, mcol, mrow, ncol, ndim, nrow;
	struct atom **a, **b, **c;
	struct atom *p1, *p2, *p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) && !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (istensor(p1) && !istensor(p2)) {
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (!istensor(p1) && istensor(p2)) {
		push(p2);
		copy_tensor();
		p2 = pop();
		n = p2->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1);
			push(p2->u.tensor->elem[i]);
			multiply();
			p2->u.tensor->elem[i] = pop();
		}
		push(p2);
		return;
	}

	k = p1->u.tensor->ndim - 1;

	ncol = p1->u.tensor->dim[k];
	mrow = p2->u.tensor->dim[0];

	if (ncol != mrow)
		stop("tensor dimensions");

	ndim = p1->u.tensor->ndim + p2->u.tensor->ndim - 2;

	if (ndim > MAXDIM)
		stop("rank exceeds max");

	//	nrow is the number of rows in p1
	//
	//	mcol is the number of columns p2
	//
	//	Example:
	//
	//	A[3][3][4] B[4][4][3]
	//
	//	  3  3				nrow = 3 * 3 = 9
	//
	//	                4  3		mcol = 4 * 3 = 12

	nrow = p1->u.tensor->nelem / ncol;
	mcol = p2->u.tensor->nelem / mrow;

	p3 = alloc_tensor(nrow * mcol);

	a = p1->u.tensor->elem;
	b = p2->u.tensor->elem;
	c = p3->u.tensor->elem;

	for (i = 0; i < nrow; i++) {
		for (j = 0; j < mcol; j++) {
			for (k = 0; k < ncol; k++) {
				push(a[i * ncol + k]);
				push(b[k * mcol + j]);
				multiply();
			}
			add_terms(ncol);
			c[i * mcol + j] = pop();
		}
	}

	if (ndim == 0) {
		push(c[0]);
		return;
	}

	// add dim info

	p3->u.tensor->ndim = ndim;

	k = 0;

	for (i = 0; i < p1->u.tensor->ndim - 1; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	for (i = 1; i < p2->u.tensor->ndim; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	push(p3);
}
