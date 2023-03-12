void
eval_rationalize(struct atom *p1)
{
	push(cadr(p1));
	eval();
	rationalize();
}

void
rationalize(void)
{
	int i, n;
	struct atom *p0, *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			rationalize();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	p2 = one;

	while (divisor(p1)) {
		p0 = pop();
		push(p0);
		push(p1);
		cancel_factor();
		p1 = pop();
		push(p0);
		push(p2);
		multiply_noexpand();
		p2 = pop();
	}

	push(p1);
	push(p2);
	reciprocate();
	multiply_noexpand();
}
