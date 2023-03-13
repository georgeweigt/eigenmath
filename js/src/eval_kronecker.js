function
eval_kronecker(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		kronecker();
		p1 = cdr(p1);
	}
}

function
kronecker()
{
	var h, i, j, k, l, m, n, p, q, p1, p2, p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1.dim.length > 2 || p2.dim.length > 2)
		stopf("kronecker");

	m = p1.dim[0];
	n = p1.dim.length == 1 ? 1 : p1.dim[1];

	p = p2.dim[0];
	q = p2.dim.length == 1 ? 1 : p2.dim[1];

	p3 = alloc_tensor();

	// result matrix has (m * p) rows and (n * q) columns

	h = 0;

	for (i = 0; i < m; i++)
		for (j = 0; j < p; j++)
			for (k = 0; k < n; k++)
				for (l = 0; l < q; l++) {
					push(p1.elem[n * i + k]);
					push(p2.elem[q * j + l]);
					multiply();
					p3.elem[h++] = pop();
				}

	// dim info

	p3.dim[0] = m * p;

	if (n * q > 1)
		p3.dim[1] = n * q;

	push(p3);
}
