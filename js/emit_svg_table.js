function
emit_svg_table(p, x, y)
{
	var dx, i, j, m, n, w;
	var cell_height, cell_depth, cell_width;

	n = p.n; // number of rows
	m = p.m; // number of columns

	y -= p.height;

	for (i = 0; i < n; i++) { // for each row

		dx = 0;

		cell_height = p.a[i * m].cell_height;
		cell_depth = p.a[i * m].cell_depth;

		y += cell_height;

		for (j = 0; j < m; j++) { // for each column
			cell_width = p.a[j].cell_width;
			w = p.a[i * m + j].width;
			emit_svg(p.a[i * m + j], x + dx + (cell_width - w) / 2, y);
			dx += cell_width;
		}

		y += cell_depth;
	}
}
