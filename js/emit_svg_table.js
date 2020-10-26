function
emit_svg_table(u, x, y)
{
	var dx, i, j, m, n, w;
	var cell_height, cell_depth, cell_width;

	n = u.n; // number of rows
	m = u.m; // number of columns

	y -= u.height;

	for (i = 0; i < n; i++) { // for each row

		dx = 0;

		cell_height = u.a[i * m].cell_height;
		cell_depth = u.a[i * m].cell_depth;

		y += cell_height;

		for (j = 0; j < m; j++) { // for each column
			cell_width = u.a[j].cell_width;
			w = u.a[i * m + j].width;
			emit_svg(u.a[i * m + j], x + dx + (cell_width - w) / 2, y);
			dx += cell_width;
		}

		y += cell_depth;
	}
}
