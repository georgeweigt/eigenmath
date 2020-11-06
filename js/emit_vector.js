function
emit_vector(u, p)
{
	var i, n, s, v;

	n = p.dim[0]; // number of rows

	// span

	s = 1;

	for (i = 1; i < p.dim.length; i++)
		s *= p.dim[i];

	v = {type:TABLE, n:n, m:1, a:[], level:0};

	for (i = 0; i < n; i++)
		emit_matrix(v, p, 1, i * s);

	emit_update_table(v);

	u.a.push(v);
}
