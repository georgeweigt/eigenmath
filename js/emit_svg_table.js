function
emit_svg_table(p, x, y)
{
	var cell_depth, cell_height, dx, i, j, m, n;

	n = p.n; // number of rows
	m = p.m; // number of columns

	dx = 0;
	y -= p.height;

	for (i = 0; i < n; i++) { // for each row

		cell_height = p.a[i * m].cell_height;
		cell_depth = p.a[i * m].cell_depth;

		y += cell_height;

		for (j = 0; j < m; j++) { // for each column
//FIXME			dx = p.a[i * m + j].dx;
			emit_svg(p.a[i * m + j], x + dx, y);
		}

		y += cell_depth;
	}
}
