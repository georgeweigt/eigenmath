function
eval_zero(p1)
{
	var i, m, n, p2;

	if (length(p1) < 2)
		stop("argument expected");

	p1 = cdr(p1);
	p2 = alloc_tensor();

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		n = pop_integer();
		if (n < 2)
			stop("dimension range error");
		p2.dim.push(n);
		m *= n;
		p1 = cdr(p1);
	}

	for (i = 0; i < m; i++)
		p1.elem[i] = zero;

	push(p1);
}
