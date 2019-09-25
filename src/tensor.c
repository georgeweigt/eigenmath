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

void
tensor_plus_tensor(void)
{
	int i, ndim, nelem;
	struct atom **a, **b, **c;

	save();

	p2 = pop();
	p1 = pop();

	// are the dimension lists equal?

	ndim = p1->u.tensor->ndim;

	if (ndim != p2->u.tensor->ndim) {
		push_symbol(NIL);
		restore();
		return;
	}

	for (i = 0; i < ndim; i++)
		if (p1->u.tensor->dim[i] != p2->u.tensor->dim[i]) {
			push_symbol(NIL);
			restore();
			return;
		}

	// create a new tensor for the result

	nelem = p1->u.tensor->nelem;

	p3 = alloc_tensor(nelem);

	p3->u.tensor->ndim = ndim;

	for (i = 0; i < ndim; i++)
		p3->u.tensor->dim[i] = p1->u.tensor->dim[i];

	// c = a + b

	a = p1->u.tensor->elem;
	b = p2->u.tensor->elem;
	c = p3->u.tensor->elem;

	for (i = 0; i < nelem; i++) {
		push(a[i]);
		push(b[i]);
		add();
		c[i] = pop();
	}

	// push the result

	push(p3);

	restore();
}

void
scalar_times_tensor(void)
{
	int i, ndim, nelem;
	struct atom **a, **b;

	save();

	p2 = pop();
	p1 = pop();

	ndim = p2->u.tensor->ndim;
	nelem = p2->u.tensor->nelem;

	p3 = alloc_tensor(nelem);

	p3->u.tensor->ndim = ndim;

	for (i = 0; i < ndim; i++)
		p3->u.tensor->dim[i] = p2->u.tensor->dim[i];

	a = p2->u.tensor->elem;
	b = p3->u.tensor->elem;

	for (i = 0; i < nelem; i++) {
		push(p1);
		push(a[i]);
		multiply();
		b[i] = pop();
	}

	push(p3);

	restore();
}

int
is_square_matrix(struct atom *p)
{
	if (istensor(p) && p->u.tensor->ndim == 2 && p->u.tensor->dim[0] == p->u.tensor->dim[1])
		return 1;
	else
		return 0;
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

// tensor p1 to the power p2

void
power_tensor(void)
{
	int i, k, n;

	// first and last dims must be equal

	k = p1->u.tensor->ndim - 1;

	if (p1->u.tensor->dim[0] != p1->u.tensor->dim[k]) {
		push_symbol(POWER);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	push(p2);

	n = pop_integer();

	if (n == ERR) {
		push_symbol(POWER);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	if (n == 0) {
		if (p1->u.tensor->ndim != 2)
			stop("power(tensor,0) with tensor rank not equal to 2");
		n = p1->u.tensor->dim[0];
		p1 = alloc_tensor(n * n);
		p1->u.tensor->ndim = 2;
		p1->u.tensor->dim[0] = n;
		p1->u.tensor->dim[1] = n;
		for (i = 0; i < n; i++)
			p1->u.tensor->elem[n * i + i] = one;
		push(p1);
		return;
	}

	if (n < 0) {
		n = -n;
		push(p1);
		inv();
		p1 = pop();
	}

	push(p1);

	for (i = 1; i < n; i++) {
		push(p1);
		inner();
		if (iszero(stack[tos - 1]))
			break;
	}
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

// Tensors with elements that are also tensors get promoted to a higher rank.

void
promote_tensor(void)
{
	int i, j, k, nelem, ndim;

	save();

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		restore();
		return;
	}

	p2 = p1->u.tensor->elem[0];

	for (i = 1; i < p1->u.tensor->nelem; i++)
		if (!compatible(p2, p1->u.tensor->elem[i]))
			stop("tensor dimensions");

	if (!istensor(p2)) {
		push(p1);
		restore();
		return;
	}

	ndim = p1->u.tensor->ndim + p2->u.tensor->ndim;

	if (ndim > MAXDIM)
		stop("rank exceeds max");

	nelem = p1->u.tensor->nelem * p2->u.tensor->nelem;

	p3 = alloc_tensor(nelem);

	p3->u.tensor->ndim = ndim;

	for (i = 0; i < p1->u.tensor->ndim; i++)
		p3->u.tensor->dim[i] = p1->u.tensor->dim[i];

	for (j = 0; j < p2->u.tensor->ndim; j++)
		p3->u.tensor->dim[i + j] = p2->u.tensor->dim[j];

	k = 0;

	for (i = 0; i < p1->u.tensor->nelem; i++) {
		p2 = p1->u.tensor->elem[i];
		for (j = 0; j < p2->u.tensor->nelem; j++)
			p3->u.tensor->elem[k++] = p2->u.tensor->elem[j];
	}

	push(p3);

	restore();
}

int
compatible(struct atom *p, struct atom *q)
{
	int i;

	if (!istensor(p) && !istensor(q))
		return 1;

	if (!istensor(p) || !istensor(q))
		return 0;

	if (p->u.tensor->ndim != q->u.tensor->ndim)
		return 0;

	for (i = 0; i < p->u.tensor->ndim; i++)
		if (p->u.tensor->dim[i] != q->u.tensor->dim[i])
			return 0;

	return 1;
}
