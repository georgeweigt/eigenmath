function
eval_outer(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		outer();
		p1 = cdr(p1);
	}
}

function
outer()
{
	var i, j, k, n, ncol, nrow, p1, p2, p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	// sync diffs

	nrow = p1.elem.length;
	ncol = p2.elem.length;

	p3 = alloc_tensor();

	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++) {
			push(p1.elem[i]);
			push(p2.elem[j]);
			multiply();
			p3.elem[i * ncol + j] = pop();
		}

	// dim info

	k = 0;

	n = p1.dim.length;

	for (i = 0; i < n; i++)
		p3.dim[k++] = p1.dim[i];

	n = p2.dim.length

	for (i = 0; i < n; i++)
		p3.dim[k++] = p2.dim[i];

	push(p3);
}
