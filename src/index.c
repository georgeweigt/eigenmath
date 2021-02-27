#include "defs.h"

void
eval_index(void)
{
	int k;

	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	while (iscons(p1)) {
		push(car(p1));
		eval();
		k = pop_integer();
		indexf(k);
		p1 = cdr(p1);
	}
}

void
indexf(int k)
{
	save();
	indexf_nib(k);
	restore();
}

void
indexf_nib(int k)
{
	int i, n;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1); // quirky, but EVA2.txt depends on it
		return;
	}

	if (k < 1 || k > p1->u.tensor->dim[0])
		stop("index error");

	k--; // make zero based

	n = p1->u.tensor->nelem / p1->u.tensor->dim[0];

	if (n == 1) {
		push(p1->u.tensor->elem[k]);
		return;
	}

	p2 = alloc_tensor(n);

	for (i = 0; i < n; i++)
		p2->u.tensor->elem[i] = p1->u.tensor->elem[k * n + i];

	n = p1->u.tensor->ndim;

	for (i = 1; i < n; i++)
		p2->u.tensor->dim[i - 1] = p1->u.tensor->dim[i];

	p2->u.tensor->ndim = n - 1;

	push(p2);
}
