function
eval_sum(p1)
{
	var h, j, k, p2, p3, t;

	t = expanding;
	expanding = 1;

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

	expanding = t;
}
