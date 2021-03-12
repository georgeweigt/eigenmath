function
kronecker()
{
	var i, j, k, l, m, n, p, q, p1, p2, p3, p4;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) || !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (p1.dim.length > 2 || p2.dim.length > 2)
		stopf("kron");

	m = p1.dim[0];
	n = p1.dim.length == 1 ? 1 : p1.dim[1];

	p = p2.dim[0];
	q = p2.dim.length == 1 ? 1 : p2.dim[1];

	p3 = alloc_tensor();

	// result matrix has (m * p) rows and (n * q) columns

	for (i = 0; i < m; i++)
		for (j = 0; j < p; j++)
			for (k = 0; k < n; k++)
				for (l = 0; l < q; l++) {
					push(p1.elem[n * i + k]);
					push(p2.elem[q * j + l]);
					multiply();
					p4 = pop();
					p3.elem.push(p4);
				}

	p3.dim[0] = m * p;

	if (n * q > 1)
		p3.dim[1] = n * q;

	push(p3);
}
