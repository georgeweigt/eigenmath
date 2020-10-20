function
emit_update_table(u)
{
	var d, h, i, j, m, n, w;

	n = u.n; // number of rows
	m = u.m; // number of columns

	// cell height and depth

	for (i = 0; i < n; i++) { // for each row
		h = 0;
		d = 0;
		for (j = 0; j < m; j++) {
			h = Math.max(h, u.a[i * m + j].height);
			d = Math.max(d, u.a[i * m + j].depth);
		}
		for (j = 0; j < m; j++) {
			u.a[i * m + j].cell_height = h + CELL_PAD;
			u.a[i * m + j].cell_depth = d + CELL_PAD;
		}
	}

	// cell width

	for (j = 0; j < m; j++) { // for each column
		w = 0;
		for (i = 0; i < n; i++)
			w = Math.max(w, u.a[i * m + j].width);
		for (i = 0; i < n; i++)
			u.a[i * m + j].cell_width = w + 2 * CELL_PAD;
	}

	// table height and depth

	h = 0;

	for (i = 0; i < n; i++)
		h += u.a[i * m].cell_height + u.a[i * m].cell_depth;

	u.height = h / 2 + X_HEIGHT;
	u.depth = h - u.height;

	// table width

	w = 0;

	for (j = 0; j < m; j++)
		w += u.a[j].cell_width;

	u.width = w + 2 * MEDIUM_SPACE;
}
