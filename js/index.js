function
index(k)
{
	var i, n, p1, p2;

	p1 = pop();

	if (!istensor(p1) || k < 1 || k > p1.dim[0])
		stopf("index error");

	k--; // make zero based

	n = p1.elem.length / p1.dim[0];

	if (n == 1) {
		push(p1.elem[k]);
		return;
	}

	p2 = alloc_tensor();

	for (i = 0; i < n; i++)
		p2.elem[i] = p1.elem[k * n + i];

	n = p1.dim.length;

	for (i = 1; i < n; i++)
		p2.dim[i - 1] = p1.dim[i];

	push(p2);
}
