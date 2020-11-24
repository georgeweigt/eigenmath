#include "app.h"

void
emit_table(double x, double y, struct atom *p)
{
	int i, j, m, n;
	double column_width, dx, elem_width, row_depth, row_height;
	struct atom *d, *h, *w, *table;

	n = (int) VAL1(p);
	m = (int) VAL2(p);

	p = cddr(p);

	table = car(p);
	h = cadr(p);
	d = caddr(p);

	for (i = 0; i < n; i++) { // for each row

		row_height = car(h)->u.d;
		row_depth = car(d)->u.d;

		y += row_height;

		dx = 0.0;

		w = cadddr(p);

		for (j = 0; j < m; j++) { // for each column
			column_width = VAL1(w);
			elem_width = WIDTH(car(table));
			emit_draw(x + dx + (column_width - elem_width) / 2.0, y, car(table));
			dx += column_width;
			w = cdr(w);
			table = cdr(table);
		}

		y += row_depth;

		h = cdr(h);
		d = cdr(d);
	}
}
