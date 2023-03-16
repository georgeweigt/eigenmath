void
eval_zero(struct atom *p1)
{
	int h, i, m, n;

	p1 = cdr(p1);
	h = tos;
	m = 1;

	while (iscons(p1)) {
		push(car(p1));
		eval();
		dupl();
		n = pop_integer();
		if (n < 2)
			stopf("zero: dim err");
		m *= n;
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n == 0) {
		push_integer(0); // scalar zero
		return;
	}

	if (n > MAXDIM)
		stopf("zero: rank err");

	p1 = alloc_tensor(m);

	for (i = 0; i < m; i++)
		p1->u.tensor->elem[i] = zero;

	// dim info

	p1->u.tensor->ndim = n;

	for (i = 0; i < n; i++)
		p1->u.tensor->dim[n - i - 1] = pop_integer();

	push(p1);
}
