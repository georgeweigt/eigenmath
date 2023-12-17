function
eval_sum(p1)
{
	var h, i, j, k, n, p2, p3;

	// sum over tensor elements?

	if (lengthf(p1) == 2) {
		push(cadr(p1));
		evalf();
		p1 = pop();
		if (!istensor(p1)) {
			push(p1);
			return;
		}
		n = p1.elem.length;
		for (i = 0; i < n; i++)
			push(p1.elem[i]);
		add_terms(n);
		return;
	}

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stopf("sum: symbol error");

	push(caddr(p1));
	evalf();
	j = pop_integer();

	push(cadddr(p1));
	evalf();
	k = pop_integer();

	p1 = caddddr(p1);

	save_symbol(p2);

	h = stack.length;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		push(p1);
		evalf();
		if (j == k)
			break;
		if (j < k)
			j++;
		else
			j--;
		if (stack.length - h == 1000)
			add_terms(1000); // to prevent stack overflow
	}

	add_terms(stack.length - h);

	restore_symbol();
}
