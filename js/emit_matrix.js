function
emit_matrix(p, d, k)
{
	var i, j, m, n, s, t, u;

	if (d == p.dim.length)
		return emit_expr(p.elem[k]);

	n = p.dim[d];
	m = p.dim[d + 1];

	// span

	s = 1;

	for (i = d + 2; i < p.dim.length; i++)
		s *= p.dim[i];

	u = {type:TABLE, n:n, m:m, a:[], height:0, width: 2 * PWIDTH};

	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			u.a.push(emit_matrix(p, d + 2, k + (i * m + j) * s));

	// cell height

	for (i = 0; i < n; i++) {
		t = 0;
		for (j = 0; j < m; j++)
			t = Math.max(t, u.a[i * m + j].height);
		for (j = 0; j < m; j++)
			u.a[i * m + j].cell_height = t;
	}

	// cell width

	for (j = 0; j < m; j++) {
		t = 0;
		for (i = 0; i < n; i++)
			t = Math.max(t, u.a[i * m + j].width);
		for (i = 0; i < n; i++)
			u.a[i * m + j].cell_width = t;
	}

	// table height

	for (i = 0; i < n; i++)
		u.height += u.a[i * m].cell_height;

	// table width

	for (j = 0; j < m; j++)
		u.width += u.a[j].cell_width;

	return u;
}
