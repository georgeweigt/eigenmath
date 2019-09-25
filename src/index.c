#include "defs.h"

void
eval_index(void)
{
	int h = tos;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}
	index_function(tos - h);
}

// tensor and indices on stack

void
index_function(int n)
{
	save();
	index_function_nib(n);
	restore();
}

void
index_function_nib(int n)
{
	int i, k, m, ndim, nelem, t;
	struct atom **s;

	s = stack + tos - n;
	p1 = s[0];

	// index of number

	if (isnum(p1)) {
		tos -= n;
		push(p1);
		return;
	}

	// index of symbol (f.e., u[2] -> u[2])

	if (!istensor(p1)) {
		list(n);
		p1 = pop();
		push_symbol(INDEX);
		push(p1);
		append();
		return;
	}

	ndim = p1->u.tensor->ndim;

	m = n - 1;

	if (m > ndim)
		stop("too many indices for tensor");

	k = 0;

	for (i = 0; i < m; i++) {
		push(s[i + 1]);
		t = pop_integer();
		if (t < 1 || t > p1->u.tensor->dim[i])
			stop("index out of range");
		k = k * p1->u.tensor->dim[i] + t - 1;
	}

	if (ndim == m) {
		tos -= n;
		push(p1->u.tensor->elem[k]);
		return;
	}

	for (i = m; i < ndim; i++)
		k = k * p1->u.tensor->dim[i] + 0;

	nelem = 1;

	for (i = m; i < ndim; i++)
		nelem *= p1->u.tensor->dim[i];

	p2 = alloc_tensor(nelem);

	p2->u.tensor->ndim = ndim - m;

	for (i = m; i < ndim; i++)
		p2->u.tensor->dim[i - m] = p1->u.tensor->dim[i];

	for (i = 0; i < nelem; i++)
		p2->u.tensor->elem[i] = p1->u.tensor->elem[k + i];

	tos -= n;
	push(p2);
}
