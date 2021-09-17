function
copy_tensor(p1)
{
	var i, n, p2;

	p2 = alloc_tensor();

	n = p1.dim.length;

	for (i = 0; i < n; i++)
		p2.dim[i] = p1.dim[i];

	n = p1.elem.length;

	for (i = 0; i < n; i++)
		p2.elem[i] = p1.elem[i];

	return p2;
}
