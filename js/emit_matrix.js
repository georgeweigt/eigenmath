function
emit_matrix(u, p, d, k)
{
	var i, j, m, n, s, t, v;

	if (d == p.dim.length) {
		u.a.push(emit_main(p.elem[k]));
		return;
	}

	n = p.dim[d];
	m = p.dim[d + 1];

	// span

	s = 1;

	for (i = d + 2; i < p.dim.length; i++)
		s *= p.dim[i];

	v = {type:TABLE, n:n, m:m, a:[], height:0, width: 2 * PWIDTH};

	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			emit_matrix(v, p, d + 2, k + (i * m + j) * s);

	// cell height

	for (i = 0; i < n; i++) {
		t = 0;
		for (j = 0; j < m; j++)
			t = Math.max(t, v.a[i * m + j].height);
		for (j = 0; j < m; j++)
			v.a[i * m + j].cell_height = t;
	}

	// cell width

	for (j = 0; j < m; j++) {
		t = 0;
		for (i = 0; i < n; i++)
			t = Math.max(t, v.a[i * m + j].width);
		for (i = 0; i < n; i++)
			v.a[i * m + j].cell_width = t;
	}

	// table height

	for (i = 0; i < n; i++)
		v.height += v.a[i * m].cell_height;

	// table width

	for (j = 0; j < m; j++)
		v.width += v.a[j].cell_width;

	u.a.push(v);
}
