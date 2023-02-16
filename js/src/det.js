function
det()
{
	var h, i, j, k, m, n, p1, p2;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	if (p1.dim.length != 2 || p1.dim[0] != p1.dim[1])
		stopf("det: square matrix expected");

	n = p1.dim[0];

	switch (n) {
	case 1:
		push(p1.elem[0]);
		return;
	case 2:
		push(p1.elem[0]);
		push(p1.elem[3]);
		multiply();
		push(p1.elem[1]);
		push(p1.elem[2]);
		multiply();
		subtract();
		return;
	}

	p2 = alloc_matrix(n - 1, n - 1);

	h = stack.length;

	for (m = 0; m < n; m++) {
		if (iszero(p1.elem[m]))
			continue;
		k = 0;
		for (i = 1; i < n; i++)
			for (j = 0; j < n; j++)
				if (j != m)
					p2.elem[k++] = p1.elem[n * i + j];
		push(p2);
		det();
		push(p1.elem[m]);
		multiply();
		if (m % 2)
			negate();
	}

	n = stack.length - h;

	if (n == 0)
		push_integer(0);
	else
		add_terms(n);
}
