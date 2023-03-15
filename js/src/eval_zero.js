function
eval_zero(p1)
{
	var i, m, n, p2;

	p1 = cdr(p1);
	p2 = alloc_tensor();

	m = 1;

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		n = pop_integer();
		if (n < 2)
			stopf("zero: dim err");
		p2.dim.push(n);
		m *= n;
		p1 = cdr(p1);
	}

	for (i = 0; i < m; i++)
		p2.elem[i] = zero;

	push(p2);
}
