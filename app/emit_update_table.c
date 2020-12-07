#include "app.h"

void
emit_update_table(int n, int m)
{
	int i, j, t;
	double d, h, w;
	double total_height, total_width;

	save();

	total_height = 0.0;
	total_width = 0.0;

	t = tos - n * m;

	// height of each row

	for (i = 0; i < n; i++) { // for each row
		h = 0.0;
		for (j = 0; j < m; j++) { // for each column
			p1 = stack[t + i * m + j];
			h = fmax(h, HEIGHT(p1));
		}
		h += TABLE_VSPACE;
		push_double(h);
		total_height += h;
	}

	list(n);
	p2 = pop();

	// depth of each row

	for (i = 0; i < n; i++) { // for each row
		d = 0.0;
		for (j = 0; j < m; j++) { // for each column
			p1 = stack[t + i * m + j];
			d = fmax(d, DEPTH(p1));
		}
		d += TABLE_VSPACE;
		push_double(d);
		total_height += d;
	}

	list(n);
	p3 = pop();

	// width of each column

	for (j = 0; j < m; j++) { // for each column
		w = 0.0;
		for (i = 0; i < n; i++) { // for each row
			p1 = stack[t + i * m + j];
			w = fmax(w, WIDTH(p1));
		}
		w += 2.0 * TABLE_HSPACE;
		push_double(w);
		total_width += w;
	}

	list(m);
	p4 = pop();

	// h, d, w for entire table

	h = total_height / 2.0 + get_operator_height(ROMAN_FONT);
	d = total_height - h;
	w = total_width + 2.0 * get_char_width(ROMAN_FONT, '(');

	list(n * m);
	p1 = pop();

	push_double(EMIT_TABLE);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(n);
	push_double(m);
	push(p1);
	push(p2);
	push(p3);
	push(p4);

	list(10);

	emit_count += 36; // alloc 36 for drawing delimiters

	restore();
}
