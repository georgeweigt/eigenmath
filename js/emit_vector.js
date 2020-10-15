function
emit_vector(p)
{
	var i, n, s, t, u;

	n = p.dim[0];

	// span

	s = 1;

	for (i = 1; i < p.dim.length; i++)
		s *= p.dim[i];

	u = {type:TABLE, n:n, m:1, a:[], height:0, width: 2 * PWIDTH};

	for (i = 0; i < n; i++)
		u.a.push(emit_matrix(p, 1, i * s));

	// cell height

	for (i = 0; i < n; i++)
		u.a[i].cell_height = u.a[i].height;

	// cell width

	t = 0;

	for (i = 0; i < n; i++)
		t = Math.max(t, u.a[i].width);

	for (i = 0; i < n; i++)
		u.a[i].cell_width = t;

	// table height

	for (i = 0; i < n; i++)
		u.height += u.a[i].height;

	// table width

	u.width += u.a[0].width;

	return u;
}
