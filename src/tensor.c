void
eval_tensor(struct atom *p1)
{
	int i;

	p1 = copy_tensor(p1);

	for (i = 0; i < p1->u.tensor->nelem; i++) {
		push(p1->u.tensor->elem[i]);
		eval();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);

	promote_tensor();
}

// tensors with elements that are also tensors get promoted to a higher rank

void
promote_tensor(void)
{
	int i, j, k, ndim1, ndim2, nelem1, nelem2;
	struct atom *p1, *p2, *p3;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	ndim1 = p1->u.tensor->ndim;
	nelem1 = p1->u.tensor->nelem;

	// check

	p2 = p1->u.tensor->elem[0];

	for (i = 1; i < nelem1; i++) {
		p3 = p1->u.tensor->elem[i];
		if (!compatible_dimensions(p2, p3))
			stopf("tensor dimensions");
	}

	if (!istensor(p2)) {
		push(p1);
		return; // all elements are scalars
	}

	ndim2 = p2->u.tensor->ndim;
	nelem2 = p2->u.tensor->nelem;

	if (ndim1 + ndim2 > MAXDIM)
		stopf("rank exceeds max");

	// alloc

	p3 = alloc_tensor(nelem1 * nelem2);

	// merge dimensions

	k = 0;

	for (i = 0; i < ndim1; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	for (i = 0; i < ndim2; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	p3->u.tensor->ndim = ndim1 + ndim2;

	// merge elements

	k = 0;

	for (i = 0; i < nelem1; i++) {
		p2 = p1->u.tensor->elem[i];
		for (j = 0; j < nelem2; j++)
			p3->u.tensor->elem[k++] = p2->u.tensor->elem[j];
	}

	push(p3);
}

int
compatible_dimensions(struct atom *p, struct atom *q)
{
	int i, n;

	if (!istensor(p) && !istensor(q))
		return 1; // both p and q are scalars

	if (!istensor(p) || !istensor(q))
		return 0; // scalar and tensor

	n = p->u.tensor->ndim;

	if (n != q->u.tensor->ndim)
		return 0;

	for (i = 0; i < n; i++)
		if (p->u.tensor->dim[i] != q->u.tensor->dim[i])
			return 0;

	return 1;
}

int
compare_tensors(struct atom *p1, struct atom *p2)
{
	int i;

	if (p1->u.tensor->ndim < p2->u.tensor->ndim)
		return -1;

	if (p1->u.tensor->ndim > p2->u.tensor->ndim)
		return 1;

	for (i = 0; i < p1->u.tensor->ndim; i++) {
		if (p1->u.tensor->dim[i] < p2->u.tensor->dim[i])
			return -1;
		if (p1->u.tensor->dim[i] > p2->u.tensor->dim[i])
			return 1;
	}

	for (i = 0; i < p1->u.tensor->nelem; i++) {
		if (equal(p1->u.tensor->elem[i], p2->u.tensor->elem[i]))
			continue;
		if (lessp(p1->u.tensor->elem[i], p2->u.tensor->elem[i]))
			return -1;
		else
			return 1;
	}

	return 0;
}

struct atom *
copy_tensor(struct atom *p1)
{
	int i;
	struct atom *p2;

	p2 = alloc_tensor(p1->u.tensor->nelem);

	p2->u.tensor->ndim = p1->u.tensor->ndim;

	for (i = 0; i < p1->u.tensor->ndim; i++)
		p2->u.tensor->dim[i] = p1->u.tensor->dim[i];

	for (i = 0; i < p1->u.tensor->nelem; i++)
		p2->u.tensor->elem[i] = p1->u.tensor->elem[i];

	return p2;
}
