function
eval_sum(p1)
{
	var h, j, k, p2;

	p1 = cdr(p1);

	p2 = car(p1);
	p1 = cdr(p1);

	if (!isusersymbol(p2))
		stop("symbol expected");

	push(car(p1));
	p1 = cdr(p1);
	evalf();
	j = pop_integer();

	push(car(p1));
	p1 = cdr(p1);
	evalf();
	k = pop_integer();

	p1 = car(p1);

	save_binding(p2);

	h = stack.length;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_binding(p2, p3);
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

	restore_binding(p2);
}
