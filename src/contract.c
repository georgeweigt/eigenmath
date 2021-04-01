#include "defs.h"

void
eval_contract(void)
{
	int t = expanding;
	expanding = 1;
	eval_contract_nib();
	expanding = t;
}

void
eval_contract_nib(void)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	if (!iscons(p1)) {
		push_integer(1);
		push_integer(2);
		contract();
		return;
	}

	while (iscons(p1)) {
		push(car(p1));
		eval();
		push(cadr(p1));
		eval();
		contract();
		p1 = cddr(p1);
	}
}

void
contract(void)
{
	save();
	contract_nib();
	restore();
}

void
contract_nib(void)
{
	int h, i, j, k, m, n, ncol, ndim, nelem, nrow;
	int index[MAXDIM];
	struct atom **a, **b;

	p3 = pop();
	p2 = pop();
	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	ndim = p1->u.tensor->ndim;

	push(p2);
	n = pop_integer();

	push(p3);
	m = pop_integer();

	if (n < 1 || n > ndim || m < 1 || m > ndim || n == m)
		stop("contract: index error");

	n--; // make zero based
	m--;

	ncol = p1->u.tensor->dim[n];
	nrow = p1->u.tensor->dim[m];

	if (ncol != nrow)
		stop("contract: unequal tensor dimensions");

	// nelem is the number of elements in result

	nelem = p1->u.tensor->nelem / ncol / nrow;

	p2 = alloc_tensor(nelem);

	a = p1->u.tensor->elem;
	b = p2->u.tensor->elem;

	for (i = 0; i < ndim; i++)
		index[i] = 0;

	for (i = 0; i < nelem; i++) {

		for (j = 0; j < ncol; j++) {
			index[n] = j;
			index[m] = j;
			k = index[0];
			for (h = 1; h < ndim; h++)
				k = k * p1->u.tensor->dim[h] + index[h];
			push(a[k]);
		}

		add_terms(ncol);

		b[i] = pop();

		// increment index

		for (j = ndim - 1; j >= 0; j--) {
			if (j == n || j == m)
				continue;
			if (++index[j] < p1->u.tensor->dim[j])
				break;
			index[j] = 0;
		}
	}

	if (nelem == 1) {
		push(b[0]);
		return;
	}

	// add dim info

	p2->u.tensor->ndim = ndim - 2;

	k = 0;

	for (i = 0; i < ndim; i++)
		if (i != n && i != m)
			p2->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	push(p2);
}
