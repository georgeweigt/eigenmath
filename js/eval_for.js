function
eval_for(p1)
{
	var j, k, p2, p3;

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

	save_binding(p2);

	for (;;) {
		set_binding(p2, j);
		p3 = p1;
		while (iscons(p3)) {
			push(car(p3));
			evalf();
			pop();
			p3 = cdr(p3);
		}
		if (j < k)
			j++;
		else if (j > k)
			j--;
		else
			break;
	}

	restore_binding(p2);

	push_symbol(NIL); // return value
}
