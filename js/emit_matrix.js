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

	emit_table_begin();

	for (i = 0; i < n; i++) {
		emit_table_row_begin();
		for (j = 0; j < m; j++) {
			emit_table_data_begin();
			k = print_matrix(p, d + 2, k);
			emit_table_data_end();
		}
		emit_table_row_end();
	}

	emit_table_end();

	return k;
}
