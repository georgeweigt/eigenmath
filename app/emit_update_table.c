#include "app.h"

void
emit_update_table(int n, int m)
{
	int i, j, t;
	double h, d, w, total_height, total_width;
	struct atom *p;

	save();

	total_height = 0.0;
	total_width = 0.0;

	t = tos - n * m;

	// row height and depth

	for (i = 0; i < n; i++) { // for each row
		h = 0.0;
		d = 0.0;
		for (j = 0; j < m; j++) { // for each column
			p = stack[t + i * m + j];
			h = fmax(h, HEIGHT(p));
			d = fmax(d, DEPTH(p));
		}
		h += TABLE_VSPACE;
		d += TABLE_VSPACE;
		push_double(h);
		push_double(d);
		total_height += h + d;
	}

	// column width

	for (j = 0; j < m; j++) { // for each column
		w = 0.0;
		for (i = 0; i < n; i++) { // for each row
			p = stack[t + i * m + j];
			w = fmax(w, WIDTH(p));
		}
		w += 2.0 * TABLE_HSPACE;
		push_double(w);
		total_width += w;
	}

	h = total_height / 2.0 + MINUS_HEIGHT;
	d = total_height - h;
	w = total_width + 2.0 * get_width(TIMES_FONT, '(');

	list(m);
	p3 = pop(); // list of column widths

	list(2 * n);
	p2 = pop(); // list of row height and depth pairs

	list(n * m);
	p1 = pop(); // list of elements

	push_double(EMIT_TABLE);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(n);
	push_double(m);
	push(p1);
	push(p2);
	push(p3);

	list(7);

	emit_count += 36; // alloc 36 floats for drawing delimiters

	restore();
}
