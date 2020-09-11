function
eval_transpose(p1)
{
	var n, m, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	if (!istensor(p2))
		stop("tensor expected");
	push(p2);

	p1 = cddr(p1);

	if (!iscons(p1)) {
		transpose(1, 2);
		return;
	}

	push(car(p1));
	evalf();
	n = pop_integer();

	push(cadr(p1));
	evalf();
	m = pop_integer();

	transpose(n, m);
}
