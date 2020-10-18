function
emit_matrix(u, p, d, k)
{
	var i, j, m, n, s, v;

	if (d == p.dim.length) {
		v = emit_new(p.elem[k]);
		u.a.push(v);
		return;
	}

	n = p.dim[d];
	m = p.dim[d + 1];

	// span

	s = 1;

	for (i = d + 2; i < p.dim.length; i++)
		s *= p.dim[i];

	v = {type:TABLE, n:n, m:m, a:[]};

	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			emit_matrix(v, p, d + 2, k + (i * m + j) * s);

	emit_update_table(v);

	u.a.push(v);
}
