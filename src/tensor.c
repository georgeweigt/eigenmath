#include "defs.h"

void
eval_tensor(void)
{
	int i;

	push(p1);
	copy_tensor();
	p1 = pop();

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
	save();
	promote_tensor_nib();
	restore();
}

void
promote_tensor_nib(void)
{
	int i, j, k, ndim1, ndim2, nelem1, nelem2;

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
			stop("tensor dimensions");
	}

	if (!istensor(p2)) {
		push(p1);
		return; // all elements are scalars
	}

	ndim2 = p2->u.tensor->ndim;
	nelem2 = p2->u.tensor->nelem;

	if (ndim1 + ndim2 > MAXDIM)
		stop("rank exceeds max");

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

// gradient of tensor p1 wrt vector p2

void
d_tensor_tensor(void)
{
	int i, j, n1, n2, ndim;
	struct atom **a, **b, **c;

	if (p2->u.tensor->ndim != 1)
		stop("vector expected");

	ndim = p1->u.tensor->ndim;

	if (ndim + 1 > MAXDIM)
		stop("rank exceeds max");

	n1 = p1->u.tensor->nelem;
	n2 = p2->u.tensor->nelem;

	p3 = alloc_tensor(n1 * n2);

	// add dim info

	p3->u.tensor->ndim = ndim + 1;

	for (i = 0; i < ndim; i++)
		p3->u.tensor->dim[i] = p1->u.tensor->dim[i];

	p3->u.tensor->dim[ndim] = n2;

	// gradient

	a = p1->u.tensor->elem;
	b = p2->u.tensor->elem;
	c = p3->u.tensor->elem;

	for (i = 0; i < n1; i++) {
		for (j = 0; j < n2; j++) {
			push(a[i]);
			push(b[j]);
			derivative();
			c[n2 * i + j] = pop();
		}
	}

	push(p3);
}

// gradient of scalar p1 wrt vector p2

void
d_scalar_tensor(void)
{
	int i, n;
	struct atom **a, **b;

	if (p2->u.tensor->ndim != 1)
		stop("vector expected");

	push(p2);
	copy_tensor();
	p3 = pop();

	a = p2->u.tensor->elem;
	b = p3->u.tensor->elem;

	n = p2->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1);
		push(a[i]);
		derivative();
		b[i] = pop();
	}

	push(p3);
}

// derivative of tensor p1 wrt scalar p2

void
d_tensor_scalar(void)
{
	int i, n;
	struct atom **a, **b;

	push(p1);
	copy_tensor();
	p3 = pop();

	a = p1->u.tensor->elem;
	b = p3->u.tensor->elem;

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(a[i]);
		push(p2);
		derivative();
		b[i] = pop();
	}

	push(p3);
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

void
copy_tensor(void)
{
	int i;

	save();

	p1 = pop();

	p2 = alloc_tensor(p1->u.tensor->nelem);

	p2->u.tensor->ndim = p1->u.tensor->ndim;

	for (i = 0; i < p1->u.tensor->ndim; i++)
		p2->u.tensor->dim[i] = p1->u.tensor->dim[i];

	for (i = 0; i < p1->u.tensor->nelem; i++)
		p2->u.tensor->elem[i] = p1->u.tensor->elem[i];

	push(p2);

	restore();
}

void
eval_dim(void)
{
	int k;

	push(cadr(p1));
	eval();
	p2 = pop();

	if (!istensor(p2)) {
		push_integer(1);
		return;
	}

	if (length(p1) == 2)
		k = 1;
	else {
		push(caddr(p1));
		eval();
		k = pop_integer();
	}

	if (k < 1 || k > p2->u.tensor->ndim)
		stop("dim 2nd arg: error");

	push_integer(p2->u.tensor->dim[k - 1]);
}

void
eval_rank(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	p1 = pop();
	if (istensor(p1))
		push_integer(p1->u.tensor->ndim);
	else
		push_integer(0);
	expanding = t;
}

void
eval_unit(void)
{
	int t = expanding;
	expanding = 1;
	eval_unit_nib();
	expanding = t;
}

void
eval_unit_nib(void)
{
	int i, n;

	push(cadr(p1));
	eval();

	n = pop_integer();

	if (n < 1)
		stop("unit: index error");

	if (n == 1) {
		push_integer(1);
		return;
	}

	p1 = alloc_matrix(n, n);

	for (i = 0; i < n; i++)
		p1->u.tensor->elem[n * i + i] = one;

	push(p1);
}

void
eval_zero(void)
{
	int t = expanding;
	expanding = 1;
	eval_zero_nib();
	expanding = t;
}

void
eval_zero_nib(void)
{
	int dim[MAXDIM], i, m, n;
	m = 1;
	n = 0;
	p1 = cdr(p1);
	while (iscons(p1)) {
		if (n == MAXDIM)
			stop("zero: rank exceeds max");
		push(car(p1));
		eval();
		i = pop_integer();
		if (i == ERR || i < 2)
			stop("zero: dimension error");
		m *= i;
		dim[n++] = i;
		p1 = cdr(p1);
	}
	if (n == 0) {
		push_integer(0);
		return;
	}
	p1 = alloc_tensor(m);
	p1->u.tensor->ndim = n;
	for (i = 0; i < n; i++)
		p1->u.tensor->dim[i] = dim[i];
	push(p1);
}
