function
adj()
{
	var col, i, j, k, n, row, p1, p2, p3;

	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 2 || p1.dim[0] != p1.dim[1])
		stop("adj: square matrix expected");

	n = p1.dim[0];

	// p2 is the adjunct matrix

	p2 = alloc_tensor();

	p2.dim[0] = n;
	p2.dim[1] = n;

	if (n == 2) {
		p2.elem[0] = p1.elem[3];
		push(p1.elem[1]);
		negate();
		p2.elem[1] = pop();
		push(p1.elem[2]);
		negate();
		p2.elem[2] = pop();
		p2.elem[3] = p1.elem[0];
		push(p2);
		return;
	}

	// p3 is for computing cofactors

	p3 = alloc_tensor();

	p3.dim[0] = n - 1;
	p3.dim[1] = n - 1;

	for (row = 0; row < n; row++) {
		for (col = 0; col < n; col++) {
			k = 0;
			for (i = 0; i < n; i++)
				for (j = 0; j < n; j++)
					if (i != row && j != col)
						p3.elem[k++] = p1.elem[n * i + j];
			push(p3);
			det();
			if ((row + col) % 2)
				negate();
			p2.elem[n * col + row] = pop(); // transpose
		}
	}

	push(p2);
}
