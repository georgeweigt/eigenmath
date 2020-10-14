function
emit_vector(p)
{
	var i, k, n;

	emit_table_begin();

	k = 0;
	n = p.dim[0];

	for (i = 0; i < n; i++) {
		emit_table_row_begin();
		emit_table_data_begin();
		k = emit_matrix(p, 1, k);
		emit_table_data_end();
		emit_table_row_end();
	}

	emit_table_end();
}
