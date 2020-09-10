function
eval_transpose(p1)
{
	var n, m, p2;

	p1 = cdr(p1);

	push(car(p1));
	p1 = cdr(p1);
	evalf();
	p2 = pop();

	if (!isusersymbol(p2))
		stop("tensor expected");

	push(p2);

	if (!iscons(p1)) {
		transpose(1, 2);
		return;
	}

	while (iscons(p1)) {

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		n = pop_integer();

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		m = pop_integer();

		if (n < 1 || n > p2.dim.length || m < 1 || n > p2.dim.length)
			stop("index out of range");

		transpose(n, m);
	}
}
