#include "defs.h"

void
eval_hadamard(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		hadamard();
		p1 = cdr(p1);
	}
	expanding = t;
}

void
hadamard(void)
{
	save();
	hadamard_nib();
	restore();
}

void
hadamard_nib(void)
{
	int i, n;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1->u.tensor->ndim != p2->u.tensor->ndim)
		stop("hadamard");

	n = p1->u.tensor->ndim;

	for (i = 0; i < n; i++)
		if (p1->u.tensor->dim[i] != p2->u.tensor->dim[i])
			stop("hadamard");

	push(p1);
	copy_tensor();
	p1 = pop();

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(p2->u.tensor->elem[i]);
		multiply();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);
}
