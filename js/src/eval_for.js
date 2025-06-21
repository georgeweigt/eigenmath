function
eval_for(p1)
{
	var j, k, p2, p3;

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stopf("for: index symbol err");

	push(caddr(p1));
	evalf();
	p3 = pop();
	if (!issmallinteger(p3))
		stopf("for: index range err");
	push(p3);
	j = pop_integer();

	push(cadddr(p1));
	evalf();
	p3 = pop();
	if (!issmallinteger(p3))
		stopf("for: index range err");
	push(p3);
	k = pop_integer();

	p1 = cddddr(p1);

	save_symbol(p2);

	breakflag = 0;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		p3 = p1;
		while (iscons(p3)) {
			push(car(p3));
			evalf();
			pop();
			p3 = cdr(p3);
			if (breakflag) {
				breakflag = 0;
				restore_symbol();
				push_symbol(NIL);
				return;
			}
		}
		if (j == k)
			break;
		if (j < k)
			j++;
		else
			j--;
	}

	restore_symbol();
	push_symbol(NIL);
}
