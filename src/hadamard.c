void
eval_hadamard(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		hadamard();
		p1 = cdr(p1);
	}
}

void
hadamard(void)
{
	int i, n;
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1->u.tensor->ndim != p2->u.tensor->ndim)
		stopf("hadamard");

	n = p1->u.tensor->ndim;

	for (i = 0; i < n; i++)
		if (p1->u.tensor->dim[i] != p2->u.tensor->dim[i])
			stopf("hadamard");

	p1 = copy_tensor(p1);

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(p2->u.tensor->elem[i]);
		multiply();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);
}
