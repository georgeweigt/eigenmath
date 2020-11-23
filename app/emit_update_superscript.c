#include "app.h"

void
emit_update_superscript(int t)
{
	int i;
	double dx, dy, h, d, w, y;

	save();

	h = 0.0;
	d = 0.0;
	w = 0.0;

	for (i = t; i < tos; i++) {
		p1 = stack[i];
		h = fmax(h, HEIGHT(p1));
		d = fmax(d, DEPTH(p1));
		w += WIDTH(p1);
	}

	// y is height of neighbor

	p1 = stack[t - 1];
	y = HEIGHT(p1);

	// adjust

	y -= (h + d) / 2.0;

	if (emit_level == 0)
		y = fmax(y, SUPERSCRIPT_HEIGHT);
	else
		y = fmax(y, SMALL_SUPERSCRIPT_HEIGHT);

	dx = 0.0;
	dy = -(y + d);

	h = y + h + d;
	d = 0.0;

	if (car(p1)->u.d == EMIT_SUBSCRIPT) {
		dx = -WIDTH(p1);
		w = fmax(0.0, w - WIDTH(p1));
	}

	list(tos - t);
	p1 = pop();

	push_double(EMIT_SUPERSCRIPT);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(dx);
	push_double(dy);
	push(p1);

	list(7);

	restore();
}
