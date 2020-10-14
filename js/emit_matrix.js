function
emit_matrix(p, d, k)
{
	var i, j, m, n;

	if (d == p.dim.length) {
		emit_expr(p.elem[k]);
		return k + 1;
	}

	n = p.dim[d];
	m = p.dim[d + 1];

	emit_table_begin(n, m); // n rows, m columns

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			emit_data_begin();
			k = print_matrix(p, d + 2, k);
			emit_data_end();
		}
	}

	emit_table_end();

	return k;
}
