function
eval_zero(p1)
{
	var h, i, m, n;

	p1 = cdr(p1);
	h = stack.length;
	m = 1;

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		dupl();
		n = pop_integer();
		if (n < 2)
			stopf("zero: dim err");
		m *= n;
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n == 0) {
		push_integer(0); // scalar zero
		return;
	}

	p1 = alloc_tensor();

	for (i = 0; i < m; i++)
		p1.elem[i] = zero;

	// dim info

	for (i = 0; i < n; i++)
		p1.dim[n - i - 1] = pop_integer();

	push(p1);
}
