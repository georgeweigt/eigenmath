void
eval_sum(struct atom *p1)
{
	int h, i, j, k, n;
	struct atom *p2, *p3;

	// sum over tensor elements?

	if (lengthf(p1) == 2) {
		push(cadr(p1));
		evalf();
		p1 = pop();
		if (!istensor(p1)) {
			push(p1);
			return;
		}
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++)
			push(p1->u.tensor->elem[i]);
		add_terms(n);
		return;
	}

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stopf("sum: first argument is not a symbol");

	push(caddr(p1));
	evalf();
	j = pop_integer();

	push(cadddr(p1));
	evalf();
	k = pop_integer();

	p1 = caddddr(p1);

	save_symbol(p2);

	h = tos;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		push(p1);
		evalg();
		if (j == k)
			break;
		if (j < k)
			j++;
		else
			j--;
		if (tos - h == 1000)
			add_terms(1000); // to prevent stack overflow
	}

	add_terms(tos - h);

	p1 = pop();
	restore_symbol();
	push(p1);
}
