function
d_tensor_tensor(p1, p2)
{
	var i, j, m, n, p3;

	p3 = alloc_tensor();

	n = p1.elem.length;
	m = p2.elem.length;

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			push(p1.elem[i]);
			push(p2.elem[j]);
			derivative();
			p3.elem[m * i + j] = pop();
		}
	}

	p3.dim = p1.dim.concat(p2.dim);

	push(p3);
}
