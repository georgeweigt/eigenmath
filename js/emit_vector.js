function
emit_vector(u, p)
{
	var i, n, s, t, v;

	n = p.dim[0]; // number of rows

	// span

	s = 1;

	for (i = 1; i < p.dim.length; i++)
		s *= p.dim[i];

	v = {type:TABLE, n:n, m:1, a:[], height:0, width: 2 * PWIDTH};

	for (i = 0; i < n; i++)
		emit_matrix(v, p, 1, i * s);

	// cell height

	for (i = 0; i < n; i++)
		v.a[i].cell_height = v.a[i].height;

	// cell width

	t = 0;

	for (i = 0; i < n; i++)
		t = Math.max(t, v.a[i].width);

	for (i = 0; i < n; i++)
		v.a[i].cell_width = t;

	// table height

	for (i = 0; i < n; i++)
		v.height += v.a[i].height;

	// table width

	v.width += v.a[0].width;

	u.a.push(v);
}
