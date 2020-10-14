function
emit_vector(p)
{
	var i, k, n;

	k = 0;
	n = p.dim[0];

	emit_table_begin(n, 1); // n rows, 1 column

	for (i = 0; i < n; i++) {
		emit_data_begin();
		k = emit_matrix(p, 1, k);
		emit_data_end();
	}

	emit_table_end();
}
