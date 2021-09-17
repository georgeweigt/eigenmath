function
minormatrix(row, col)
{
	var i, j, k, m, n, p1, p2;

	p1 = pop();

	n = p1.dim[0];
	m = p1.dim[1];

	if (n == 2 && m == 2) {
		if (row == 1) {
			if (col == 1)
				push(p1.elem[3]);
			else
				push(p1.elem[2]);
		} else {
			if (col == 1)
				push(p1.elem[1]);
			else
				push(p1.elem[0]);
		}
		return;
	}

	p2 = alloc_tensor();

	if (n == 2)
		p2.dim[0] = m - 1;

	if (m == 2)
		p2.dim[0] = n - 1;

	if (n > 2 && m > 2) {
		p2.dim[0] = n - 1;
		p2.dim[1] = m - 1;
	}

	row--;
	col--;

	k = 0;

	for (i = 0; i < n; i++) {

		if (i == row)
			continue;

		for (j = 0; j < m; j++) {

			if (j == col)
				continue;

			p2.elem[k++] = p1.elem[m * i + j];
		}
	}

	push(p2);
}
