void
eval_numerator(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	numerator();
}

void
numerator(void)
{
	int i, n;
	struct atom *p1;
	p1 = pop();
	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			numerator();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}
	while (numden_find_divisor(p1)) {
		push(p1);
		numden_cancel_factor();
		p1 = pop();
	}
	push(p1);
}
