#include "app.h"

void
emit_update_subscript(int t)
{
	int i;
	double h, d, w, dx, dy;

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

	if (emit_level == 0) {
		h = get_char_height(TIMES_FONT);
		d += SUBSCRIPT_DEPTH;
		dx = 0;
		dy = SUBSCRIPT_DEPTH;
	} else {
		h = get_char_height(SMALL_TIMES_FONT);
		d += SMALL_SUBSCRIPT_DEPTH;
		dx = 0;
		dy = SMALL_SUBSCRIPT_DEPTH;
	}

	list(tos - t);
	p1 = pop();

	push_double(EMIT_SUBSCRIPT);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(dx);
	push_double(dy);
	push(p1);

	list(7);

	restore();
}
