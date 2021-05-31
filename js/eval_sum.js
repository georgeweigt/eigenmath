function
eval_sum(p1)
{
	var h, i, j, k, n, p2, p3;

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
		stopf("symbol expected");

	p1 = cddr(p1);

	push(car(p1));
	evalf();
	j = pop_integer();

	push(cadr(p1));
	evalf();
	k = pop_integer();

	p1 = caddr(p1);

	save_symbol(p2);

	h = stack.length;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		push(p1);
		evalf();
		if (j < k)
			j++;
		else if (j > k)
			j--;
		else
			break;
	}

	add_terms(stack.length - h);

	restore_symbol(p2);
}
