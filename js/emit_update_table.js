function
emit_update_table(u)
{
	var d, h, i, j, m, n, w;

	n = u.n; // number of rows
	m = u.m; // number of columns

	// row height and depth

	for (i = 0; i < n; i++) { // for each row
		h = 0;
		d = 0;
		for (j = 0; j < m; j++) { // for each column
			h = Math.max(h, u.a[i * m + j].height);
			d = Math.max(d, u.a[i * m + j].depth);
		}
		u.a[i * m].cell_height = h + TABLE_VSPACE;
		u.a[i * m].cell_depth = d + TABLE_VSPACE;
	}

	// column width

	for (j = 0; j < m; j++) { // for each column
		w = 0;
		for (i = 0; i < n; i++) // for each row
			w = Math.max(w, u.a[i * m + j].width); // w is max width in column
		u.a[j].cell_width = w + 2 * TABLE_HSPACE;
	}

	// table height and depth

	h = 0;

	for (i = 0; i < n; i++) // for each row
		h += u.a[i * m].cell_height + u.a[i * m].cell_depth;

	u.height = h / 2 + MINUS_HEIGHT;
	u.depth = h - u.height;

	// table width

	w = 0;

	for (j = 0; j < m; j++)
		w += u.a[j].cell_width;

	u.width = w + 2 * emit_delim_width(u);
}
